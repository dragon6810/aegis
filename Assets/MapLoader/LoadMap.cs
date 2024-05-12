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
using UnityEditor;
using Unity.VisualScripting;
using System.Linq;
using TriangleNet.Geometry;
using TriangleNet.Topology;
using TriangleNet.Meshing;


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

    List<plane_t> planes;
    List<face_t> faces;

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

        public Vector3 toVec3()
        {
            return new Vector3(x, y, z);
        }
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

    const string testname = "maps/c1a2.bsp";
    public List<Vector3> vertices;
    List<edge_t> edges;
    List<int> surfedges;

    void Start()
    {
        loadMap(testname);
    }

    void Update()
    {
        
    }

    // Calculate normal of the triangle from three points
    private Vector3 CalculateNormal(Vector3 a, Vector3 b, Vector3 c)
    {
        Vector3 ab = b - a;
        Vector3 ac = c - a;
        return Vector3.Cross(ab, ac).normalized;
    }

    // Check if normal is consistent with winding order, if not, flip winding
    private Vector3[] CorrectWinding(Vector3 a, Vector3 b, Vector3 c, Vector3 normal)
    {
        Vector3 calculatedNormal = CalculateNormal(a, b, c);
        if (Vector3.Dot(calculatedNormal, normal) < 0)
        {
            // If the dot product is negative, the winding is opposite to the normal
            return new Vector3[] { c, b, a }; // Swap b and c to flip winding
        }
        return new Vector3[] { a, b, c };
    }

    // Method to check the orientation of the triangle formed by three points
    private bool IsTriangleOrientedClockwise(Vector3 a, Vector3 b, Vector3 c)
    {
        // Calculation assuming Vector3.X and Vector3.Z are the 2D coordinates
        return (b.x - a.x) * (c.z - a.z) - (c.x - a.x) * (b.z - a.z) < 0;
    }

    // Method to ensure the index wraps around the list correctly
    private int ClampListIndex(int index, int count)
    {
        if (index < 0) return count - 1;
        if (index >= count) return 0;
        return index;
    }

    // Method to check if a point is inside the triangle formed by three vertices
    private bool IsPointInTriangle(Vector3 point, Vector3 a, Vector3 b, Vector3 c)
    {
        float area = 0.5f * (-b.z * c.x + a.z * (-b.x + c.x) + a.x * (b.z - c.z) + b.x * c.z);
        float s = 1f / (2f * area) * (a.z * c.x - a.x * c.z + (c.z - a.z) * point.x + (a.x - c.x) * point.z);
        float t = 1f / (2f * area) * (a.x * b.z - a.z * b.x + (a.z - b.z) * point.x + (b.x - a.x) * point.z);

        return s >= 0 && t >= 0 && 1 - s - t >= 0;
    }

    // Main method to triangulate a concave polygon given vertices in counterclockwise order
    public List<Vector3[]> TriangulateConcavePolygon(List<Vector3> points, Vector3 normal)
    {
        if(points.Count == 3)
            return new List<Vector3[]> {points.ToArray()};

        Vector3 center = Vector3.zero;
        foreach(Vector3 point in points)
            center += point;
        
        center /= points.Count;

        //normal = CalculateNormal(points[0], points[1], points[2]);

        Vector3 right;
        if (normal == Vector3.up || normal == Vector3.down)
            right = Vector3.Cross(normal, Vector3.forward);
        else
            right = Vector3.Cross(normal, Vector3.up);

        if (right == Vector3.zero)
            right = Vector3.Cross(normal, Vector3.right);

        Vector3 up = Vector3.Cross(right, normal);

        Matrix4x4 toWorld = new Matrix4x4
        (
            new Vector4(right.x, right.y, right.z, 0.0F),
            new Vector4(up.x, up.y, up.z, 0.0F),
            new Vector4(normal.x, normal.y, normal.z, 0.0F),
            new Vector4(0.0F, 0.0F, 0.0F, 1.0F)
        );

        IPolygon polygon = new Polygon();
        int id = 0;
        foreach (Vector3 point in points)
        {
            Vector4 transformedPoint = toWorld * new Vector4(point.x - center.x, point.y - center.y, point.z - center.z, 1.0F);
            polygon.Points.Add(new Vertex((double) transformedPoint.x, (double) transformedPoint.y) { ID = id++ });
        }

        IMesh mesh = polygon.Triangulate();

        // Convert Triangle.NET mesh triangles back into Unity Vector3 triangles
        List<Vector3[]> triangles = new List<Vector3[]>();
        foreach (Triangle triangle in mesh.Triangles)
        {
            Vector3[] triPoints = new Vector3[3];
            triPoints[0] = new Vector3((float)triangle.GetVertex(0).X, (float)triangle.GetVertex(0).Y, 0);
            triPoints[1] = new Vector3((float)triangle.GetVertex(1).X, (float)triangle.GetVertex(1).Y, 0);
            triPoints[2] = new Vector3((float)triangle.GetVertex(2).X, (float)triangle.GetVertex(2).Y, 0);

            Vector4 p0 = toWorld.transpose * new Vector4(triPoints[0].x, triPoints[0].y, triPoints[0].z, 1.0F);
            Vector4 p1 = toWorld.transpose * new Vector4(triPoints[1].x, triPoints[1].y, triPoints[1].z, 1.0F);
            Vector4 p2 = toWorld.transpose * new Vector4(triPoints[2].x, triPoints[2].y, triPoints[2].z, 1.0F);
            triPoints[0] = new Vector3(p0.x + center.x, p0.y + center.y, p0.z + center.z);
            triPoints[1] = new Vector3(p1.x + center.x, p1.y + center.y, p1.z + center.z);
            triPoints[2] = new Vector3(p2.x + center.x, p2.y + center.y, p2.z + center.z);

            triangles.Add(triPoints);
        }

        return triangles;
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

        planes = DeserializePlanes(bytes, header.lumps[LUMP_PLANES].offset, header.lumps[LUMP_PLANES].size);

        this.vertices = DeserializeVertices(bytes, header.lumps[LUMP_VERTICES].offset, header.lumps[LUMP_VERTICES].size);

        edges = DeserializeEdges(bytes, header.lumps[LUMP_EDGES].offset, header.lumps[LUMP_EDGES].size);
        surfedges = DeserializeSurfedges(bytes, header.lumps[LUMP_SURFEDGES].offset, header.lumps[LUMP_SURFEDGES].size);
        faces = DeserializeFaces(bytes, header.lumps[LUMP_FACES].offset, header.lumps[LUMP_FACES].size);

        List<Vector3> meshVertices = new List<Vector3>();
        List<int> meshIndices = new List<int>();

        Debug.Log(faces.Count + " faces.");

        for(int i = 0; i < faces.Count; i++)
        {
            face_t face = faces[i];
            List<Vector3> polyVerts = new List<Vector3>();

            for(int s = (int) face.iFirstEdge; s < face.nEdges + face.iFirstEdge; s++)
            {
                edge_t edge = edges[Math.Abs(surfedges[s])];
                Vector3[] verts = {vertices[edge.v1], vertices[edge.v2]};
                if(surfedges[s] < 0)
                {
                    Vector3 temp = verts[0];
                    verts[0] = verts[1];
                    verts[1] = temp;
                }

                polyVerts.Add(swizzleToXZY(verts[0]));
            }

            Vector3 normal = swizzleToXZY(planes[faces[i].iPlane].vNormal.toVec3());
            if(faces[i].nPlaneSide != 0)
                normal *= -1.0F;

            List<Vector3[]> triangulated = TriangulateConcavePolygon(polyVerts, normal);
            for(int t = 0; t < triangulated.Count; t++)
                triangulated[t] = CorrectWinding(triangulated[t][0], triangulated[t][1], triangulated[t][2], normal);

            for(int t = 0; t < triangulated.Count; t++)
            {
                for(int v = 0; v < 3; v++)
                {
                    meshVertices.Add(triangulated[t][v]);
                    meshIndices.Add(meshIndices.Count);
                }
            }
        }

        MeshFilter mf = this.GetComponent<MeshFilter>();
        Mesh mesh = new Mesh();
        mf.mesh = mesh;
        mesh.vertices = meshVertices.ToArray();
        mesh.triangles = meshIndices.ToArray();
        mesh.RecalculateNormals();
        mesh.RecalculateBounds();
        mesh.RecalculateTangents();
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

    public static unsafe float Int32BitsToSingle(int value) 
    {
        return *(float*)(&value);
    }

    public List<plane_t> DeserializePlanes(byte[] data, int offset, int size)
    {
        const int planeSize = 20;

        int numPlanes = size / planeSize;

        List<plane_t> planes = new List<plane_t>();

        for(int i = 0; i < numPlanes; i++)
        {
            int currentOffset = offset + i * planeSize;
            plane_t plane;

            plane.vNormal.x = Int32BitsToSingle(BitConverter.ToInt32(data, currentOffset));
            currentOffset += sizeof(float);
            plane.vNormal.y = Int32BitsToSingle(BitConverter.ToInt32(data, currentOffset));
            currentOffset += sizeof(float);
            plane.vNormal.z = Int32BitsToSingle(BitConverter.ToInt32(data, currentOffset));
            currentOffset += sizeof(float);

            if(plane.vNormal.toVec3().Equals(Vector3.zero))
            {
                Debug.Log("Null normal plane");
            }

            plane.fDist = Int32BitsToSingle(BitConverter.ToInt32(data, currentOffset));
            currentOffset += sizeof(float);
            plane.nType = BitConverter.ToInt32(data, currentOffset);
            currentOffset += sizeof(int);

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

    public List<Int32> DeserializeSurfedges(byte[] data, int offset, int size)
    {
        const int surfedgeSize = sizeof(Int32);

        int numSurfedges = size / surfedgeSize;

        List<Int32> surfedges = new List<Int32>();
        for(int i = 0; i < numSurfedges; i++)
        {
            int currentOffset = offset + i * surfedgeSize;

            surfedges.Add(BitConverter.ToInt32(data, currentOffset));
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
            for(int j = 0; j < 4; j++)
            {
                face.nStyles[j] = (byte) BitConverter.ToChar(data, currentOffset);
                currentOffset += sizeof(byte);
            }
            face.nLightmapOffset = BitConverter.ToUInt32(data, currentOffset);
            currentOffset += sizeof(int);

            faces.Add(face);
        }

        return faces;
    }

    Vector3 swizzleToXZY(Vector3 v)
    {
        float temp = v.y;
        v.y = v.z;
        v.z = temp;
        return v;
    }
/*
    #if UNITY_EDITOR
    void OnDrawGizmos()
    {
        if(!Application.isPlaying)
            return;

        foreach(face_t face in faces)
        {
            plane_t plane = planes[face.iPlane];
            Vector3 normal = plane.vNormal.toVec3().normalized;
            if(face.nPlaneSide != 0)
                normal *= -1.0F;

            if(normal == Vector3.zero)
                continue;

            Vector3 center = Vector3.zero;

            for(int s = (int) face.iFirstEdge; s < face.nEdges + face.iFirstEdge; s++)
            {
                edge_t edge = edges[Math.Abs(surfedges[s])];
                Vector3[] verts = {vertices[edge.v1], vertices[edge.v2]};
                if(surfedges[s] < 0)
                    verts.Reverse();

                center += verts[0];
            }

            center /= (float) face.nEdges;

            Vector3 bitangent = Vector3.up;
            Vector3 tangent = Vector3.Cross(bitangent, normal);
            bitangent = Vector3.Cross(tangent, normal);

            float minBoundsTangent = 0.0F;
            float minBoundsBitangent = 0.0F;

            for(int s = (int) face.iFirstEdge; s < face.nEdges + face.iFirstEdge; s++)
            {
                edge_t edge = edges[Math.Abs(surfedges[s])];
                Vector3 vPos;
                if(surfedges[s] < 0)
                    vPos = vertices[edge.v2] - center;
                else
                    vPos = vertices[edge.v1] - center;

                float tanVal = Mathf.Abs(Vector3.Dot(vPos, tangent));
                float bitanVal = Mathf.Abs(Vector3.Dot(vPos, bitangent));

                minBoundsTangent = Mathf.Max(minBoundsTangent, tanVal);
                minBoundsBitangent = Mathf.Max(minBoundsBitangent, bitanVal);
            }

            Vector3[] rectangleCorners = new Vector3[4];
            rectangleCorners[0] = swizzleToXZY(center - tangent * minBoundsTangent - bitangent * minBoundsBitangent);
            rectangleCorners[1] = swizzleToXZY(center + tangent * minBoundsTangent - bitangent * minBoundsBitangent);
            rectangleCorners[2] = swizzleToXZY(center + tangent * minBoundsTangent + bitangent * minBoundsBitangent);
            rectangleCorners[3] = swizzleToXZY(center - tangent * minBoundsTangent + bitangent * minBoundsBitangent);

            Handles.color = Color.white;
            Handles.DrawSolidRectangleWithOutline(rectangleCorners, new Color(0.5f, 0.5f, 0.5f, 0.1f), Color.black);
            Handles.color = Color.blue;
            Handles.ArrowHandleCap(0, swizzleToXZY(center), Quaternion.LookRotation(swizzleToXZY(normal), Vector3.up), 10.0F, EventType.Repaint);

            for(int s = (int) face.iFirstEdge; s < face.nEdges + face.iFirstEdge; s++)
            {
                edge_t edge = edges[Math.Abs(surfedges[s])];
                Vector3[] verts = {vertices[edge.v1], vertices[edge.v2]};
                Vector3 vPos;
                if(surfedges[s] < 0)
                    verts.Reverse();

                Handles.color = Color.white;
                Handles.DrawLine(verts[0], verts[1]);
            }
        }

        foreach(edge_t edge in edges)
        {
            Vector3 v1 = swizzleToXZY(vertices[edge.v1]);
            Vector3 v2 = swizzleToXZY(vertices[edge.v2]);

            Handles.color = Color.white;
            Handles.DrawLine(v1, v2);
        }

        foreach(Vector3 v in vertices)
        {
            Handles.color = Color.yellow;
            Handles.SphereHandleCap(0, swizzleToXZY(v), Quaternion.identity, 1.0F, EventType.Repaint);
        }
    }
    #endif
*/
}