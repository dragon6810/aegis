using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using System;
using System.Runtime.InteropServices;
using System.Text;
using System.Text.RegularExpressions;
using System.Runtime.CompilerServices;
using System.ComponentModel;

[RequireComponent(typeof(MeshFilter), typeof(MeshRenderer))]
public class LoadMap : MonoBehaviour
{
    const int LUMP_ENTITIES = 0;
    const int LUMP_PLANES = 1;
    const int LUMP_TEXTURES = 2;
    const int LUMP_VERTICES = 3;
    const int LUMP_VISIBILITY = 4;
    const int LUMP_NODES = 5;
    const int LUMP_TEXINFO = 6;
    const int LUMP_FACES = 7;
    const int LUMP_LIGHTING = 8;
    const int LUMP_CLIPNODES = 9;
    const int LUMP_LEAVES = 10;
    const int LUMP_MARKSURFACES = 11;
    const int LUMP_EDGES = 12;
    const int LUMP_SURFEDGES = 13;
    const int LUMP_MODELS = 14;
    const int HEADER_LUMPS = 15;

    const int MAX_MAP_HULLS =        4;

    const int MAX_MAP_MODELS =       400;
    const int MAX_MAP_BRUSHES =      4096;
    const int MAX_MAP_ENTITIES =     1024;
    const int MAX_MAP_ENTSTRING =    (128*1024);

    const int MAX_MAP_PLANES =       32767;
    const int MAX_MAP_NODES =        32767;
    const int MAX_MAP_CLIPNODES =    32767;
    const int MAX_MAP_LEAFS =        8192;
    const int MAX_MAP_VERTS =        65535;
    const int MAX_MAP_FACES =        65535;
    const int MAX_MAP_MARKSURFACES = 65535;
    const int MAX_MAP_TEXINFO =      8192;
    const int MAX_MAP_EDGES =        256000;
    const int MAX_MAP_SURFEDGES =    512000;
    const int MAX_MAP_TEXTURES =     512;
    const int MAX_MAP_MIPTEX =       0x200000;
    const int MAX_MAP_LIGHTING =     0x200000;
    const int MAX_MAP_VISIBILITY =   0x200000;

    const int MAX_MAP_PORTALS =     65536;

    const int MAX_KEY =   32;
    const int MAX_VALUE = 1024;

    const int PLANE_X =    0;  // Plane is perpendicular to given axis
    const int PLANE_Y =    1;
    const int PLANE_Z =    2;
    const int PLANE_ANYX = 3;  // Non-axial plane is snapped to the nearest
    const int PLANE_ANYY = 4;
    const int PLANE_ANYZ = 5;

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct lump_t
    {
        public int offset; // File offset to data
        public int size;   // Length of data
    }

    public struct header_t
    {
        public int version;    // Must be 30 for a valid HL BSP file
        public lump_t[] lumps;   // Memory block for lumps

        public header_t(int version)
        {
            this.version = version;
            lumps = new lump_t[HEADER_LUMPS];
        }
    }

    public class Entity
    {
        public Dictionary<string, string> Properties { get; set; } = new Dictionary<string, string>();

        public override string ToString()
        {
            var builder = new StringBuilder();
            builder.AppendLine("{");
            foreach (var prop in Properties)
            {
                builder.AppendLine($"\"{prop.Key}\" \"{prop.Value}\"");
            }
            builder.AppendLine("}");
            return builder.ToString();
        }
    }

    public struct vector3_t
    {
        public float x, y, z;
    };

    public struct plane_t
    {
        public vector3_t vNormal; // The planes normal vector
        public float fDist;     // Plane equation is: vNormal * X = fDist
        public int nType;       // Plane type, see constants
    };

    public struct edge_t
    {
        public UInt16 v1;
        public UInt16 v2;
    };

    const int faceSize = sizeof(UInt16) * 4 + sizeof(UInt32) * 2 + sizeof(byte) * 4;
    public struct face_t
    {
        public UInt16 iPlane;          // Plane the face is parallel to
        public UInt16 nPlaneSide;      // Set if different normals orientation
        public UInt32 iFirstEdge;      // Index of the first surfedge
        public UInt16 nEdges;          // Number of consecutive surfedges
        public UInt16 iTextureInfo;    // Index of the texture info structure
        public byte[] nStyles;         // Specify lighting styles
        public UInt32 nLightmapOffset; // Offsets into the raw lightmap data; if less than zero, then a lightmap was not baked for the given face.

        public face_t(int foo)
        {
            this.iPlane = 0;
            this.nPlaneSide = 0;
            this.iFirstEdge = 0;
            this.nEdges = 0;
            this.iTextureInfo = 0;
            this.nLightmapOffset = 0;
            this.nStyles = new byte[4];
        }
    };

    const string testname = "maps/c4a1b.bsp";

    public List<plane_t> planes;
    public List<Vector3> vertices;

    void Start()
    {
        loadMap(testname);
    }

    void Update()
    {
        
    }

    void loadMap(String filename)
    {
        if (!File.Exists(filename))
        {
            Debug.LogError("Failed to find map " + filename + ".");
            return;
        }

        byte[] bytes = File.ReadAllBytes(filename);
        header_t header = DeserializeHeader(bytes);
        Debug.Log("File version: " + header.version + ".");

        // Assuming the lumps array is properly filled and LUMP_ENTITIES is the correct index for the entities lump
        int entityStart = header.lumps[LUMP_ENTITIES].offset;
        int entitySize = header.lumps[LUMP_ENTITIES].size;
        List<Entity> entities = ParseEntities(bytes, entityStart, entitySize);

        List<plane_t> planes = DeserializePlanes(bytes, header.lumps[LUMP_PLANES].offset, header.lumps[LUMP_PLANES].size);

        this.vertices = DeserializeVertices(bytes, header.lumps[LUMP_VERTICES].offset, header.lumps[LUMP_VERTICES].size);

        List<edge_t> edges = DeserializeEdges(bytes, header.lumps[LUMP_EDGES].offset, header.lumps[LUMP_EDGES].size);
        List<UInt32> surfedges = DeserializeSurfedges(bytes, header.lumps[LUMP_SURFEDGES].offset, header.lumps[LUMP_SURFEDGES].size);

        MeshFilter mf = this.GetComponent<MeshFilter>();
        Mesh mesh = new Mesh();
        mf.mesh = mesh;
        mesh.vertices = this.vertices.ToArray();

        List<int> indices = new List<int>();
        for(int i = 0; i < this.vertices.Count; i++)
            indices.Add(i);

        mesh.triangles = indices.ToArray();
    }

    header_t DeserializeHeader(byte[] data)
    {
        header_t header = new header_t(30);
        int currentOffset = 0;

        // Assuming the version is the first 4 bytes as an int
        header.version = BitConverter.ToInt32(data, currentOffset);
        currentOffset += sizeof(int);

        // Deserialize each Lump
        header.lumps = new lump_t[HEADER_LUMPS];
        for (int i = 0; i < HEADER_LUMPS; i++)
        {
            header.lumps[i].offset = BitConverter.ToInt32(data, currentOffset);
            currentOffset += sizeof(int);
            header.lumps[i].size = BitConverter.ToInt32(data, currentOffset);
            currentOffset += sizeof(int);
        }

        return header;
    }

    public List<Entity> ParseEntities(byte[] data, int start, int size)
    {
        string entityText = Encoding.ASCII.GetString(data, start, size);
        List<Entity> entities = new List<Entity>();
        int cursor = 0;

        while (cursor < entityText.Length)
        {
            int startIdx = entityText.IndexOf('{', cursor);
            if (startIdx == -1) break;

            int endIdx = entityText.IndexOf('}', startIdx);
            if (endIdx == -1) break;

            string entityData = entityText.Substring(startIdx + 1, endIdx - startIdx - 1);
            Entity entity = new Entity();

            // Parse key-value pairs within the entity data
            var matches = Regex.Matches(entityData, "\"(.*?)\"\\s*\"(.*?)\"");
            foreach (Match match in matches)
            {
                if (match.Groups.Count == 3)
                {
                    string key = match.Groups[1].Value;
                    string value = match.Groups[2].Value;
                    entity.Properties[key] = value;
                }
            }

            entities.Add(entity);
            cursor = endIdx + 1;
        }

        return entities;
    }

    public List<plane_t> DeserializePlanes(byte[] data, int offset, int size)
    {
        const int planeSize = 16;

        int numPlanes = size / planeSize;

        List<plane_t> planes = new List<plane_t>();

        for(int i = 0; i < numPlanes; i++)
        {
            int currentOffset = offset + i * planeSize;
            plane_t plane;

            plane.vNormal.x = BitConverter.Int32BitsToSingle(BitConverter.ToInt32(data, currentOffset));
            plane.vNormal.y = BitConverter.Int32BitsToSingle(BitConverter.ToInt32(data, currentOffset + sizeof(float)));
            plane.vNormal.z = BitConverter.Int32BitsToSingle(BitConverter.ToInt32(data, currentOffset + sizeof(float) * 2));

            plane.fDist = BitConverter.Int32BitsToSingle(BitConverter.ToInt32(data, currentOffset + sizeof(float) * 3));
            plane.nType = BitConverter.ToInt32(data, currentOffset + sizeof(float) * 4);

            planes.Add(plane);
        }

        return planes;
    }

    public List<Vector3> DeserializeVertices(byte[] data, int offset, int size)
    {
        const int vec3Size = sizeof(float) * 3;

        int numVerts = size / vec3Size;

        List<Vector3> vertices = new List<Vector3>();

        for(int i = 0; i < numVerts; i++)
        {
            int currentOffset = offset + i * vec3Size;

            Vector3 vertex;
            vertex.x = BitConverter.Int32BitsToSingle(BitConverter.ToInt32(data, currentOffset));
            vertex.y = BitConverter.Int32BitsToSingle(BitConverter.ToInt32(data, currentOffset + sizeof(float)));
            vertex.z = BitConverter.Int32BitsToSingle(BitConverter.ToInt32(data, currentOffset + sizeof(float) * 2));

            vertices.Add(vertex);
        }

        return vertices;
    }

    public List<edge_t> DeserializeEdges(byte[] data, int offset, int size)
    {
        const int edgeSize = sizeof(UInt16) * 2;

        int numEdges = size / edgeSize;

        List<edge_t> edges = new List<edge_t>();

        for(int i = 0; i < numEdges; i++)
        {
            int currentOffset = offset + i * edgeSize;

            edge_t edge;

            edge.v1 = BitConverter.ToUInt16(data, currentOffset);
            edge.v2 = BitConverter.ToUInt16(data, currentOffset + sizeof(UInt16));

            edges.Add(edge);
        }

        return edges;
    }

    public List<UInt32> DeserializeSurfedges(byte[] data, int offset, int size)
    {
        const int surfedgeSize = sizeof(UInt32);

        int numSurfedges = size / surfedgeSize;

        List<UInt32> surfedges = new List<UInt32>();
        for(int i = 0; i < numSurfedges; i++)
        {
            int currentOffset = offset + i * surfedgeSize;

            surfedges.Add(BitConverter.ToUInt32(data, currentOffset));
        }

        return surfedges;
    }

    public List<face_t> DeserializeFaces(byte[] data, int offset, int size)
    {
        int numFaces = size / faceSize;

        List<face_t> faces = new List<face_t>();
        for(int i = 0; i < numFaces; i++)
        {
            int currentOffset = offset + i * faceSize;

            face_t face = new(0);

            face.iPlane = BitConverter.ToUInt16(data, currentOffset);
            currentOffset += sizeof(UInt16);
            face.nPlaneSide = BitConverter.ToUInt16(data, currentOffset);
            currentOffset += sizeof(UInt16);
            face.iFirstEdge = BitConverter.ToUInt32(data, currentOffset);
            currentOffset += sizeof(UInt32);
            face.nEdges = BitConverter.ToUInt16(data, currentOffset);
            currentOffset += sizeof(UInt16);
            face.iTextureInfo = BitConverter.ToUInt16(data, currentOffset);
            currentOffset += sizeof(UInt16);
            face.nLightmapOffset = BitConverter.ToUInt32(data, currentOffset);
            currentOffset += sizeof(UInt32);
            for(int j = 0; j < 4; j++)
            {
                face.nStyles[j] = (byte) BitConverter.ToChar(data, currentOffset);
                currentOffset += sizeof(byte);
            }
            face.nLightmapOffset = BitConverter.ToUInt32(data, currentOffset);
            currentOffset += sizeof(UInt32);
        }
    }
}