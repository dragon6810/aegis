#include <bsplib.h>

#include <utilslib.h>

#define BSPFILE_MAGIC "BSP"
#define BSPFILE_VERSION 2025

typedef enum
{
    LUMP_INFO=0,
    LUMP_MODELS,
    LUMP_CNODES,
    LUMP_CLEAVES,
    LUMP_MPORTALS,
    LUMP_PORTALS,
    LUMP_RNODES,
    LUMP_RLEAFS,
    LUMP_MRSURFS,
    LUMP_RSURFS,
    LUMP_MVERTS,
    LUMP_VERTS,
    LUMP_ENTSTRING,
    LUMP_COUNT,
} lump_e;

typedef struct lump_s
{
    uint64_t offs;
    uint64_t size;
} lump_t;

typedef struct header_s
{
    char magic[4];
    uint32_t version;
    lump_t lumps[LUMP_COUNT];
} header_t;

void Bsplib::BspFile::Write(const char* path)
{
    const char nullterm = 0;

    FILE *ptr;
    header_t header;
    uint64_t curoffs;

    UTILS_ASSERT(path);

    ptr = fopen(path, "wb");
    if(!ptr)
    {
        fprintf(stderr, "can't open file for writing \"%s\".\n", path);
        return;
    }

    header = {};
    strcpy(header.magic, "BSP");
    header.version = 2025;

    #define LUMP_OFFS(lump, len)      \
    header.lumps[(lump)].offs = curoffs; \
    header.lumps[(lump)].size = (len);    \
    curoffs += (len)

    curoffs = 0;
    LUMP_OFFS(LUMP_INFO, sizeof(bspinfo_t));
    LUMP_OFFS(LUMP_MODELS, models.size() * sizeof(model_t));
    LUMP_OFFS(LUMP_CNODES, cnodes.size() * sizeof(cnode_t));
    LUMP_OFFS(LUMP_CLEAVES, cleaves.size() * sizeof(cleaf_t));
    LUMP_OFFS(LUMP_MPORTALS, mportals.size() * sizeof(mportal_t));
    LUMP_OFFS(LUMP_PORTALS, portals.size() * sizeof(portal_t));
    LUMP_OFFS(LUMP_RNODES, rnodes.size() * sizeof(rnode_t));
    LUMP_OFFS(LUMP_RLEAFS, rleafs.size() * sizeof(rleaf_t));
    LUMP_OFFS(LUMP_MRSURFS, mrsurfs.size() * sizeof(mrsurf_t));
    LUMP_OFFS(LUMP_RSURFS, rsurfs.size() * sizeof(rsurf_t));
    LUMP_OFFS(LUMP_MVERTS, mverts.size() * sizeof(mvert_t));
    LUMP_OFFS(LUMP_VERTS, verts.size() * sizeof(vec_t));
    LUMP_OFFS(LUMP_ENTSTRING, entstring.size() + 1);

    #undef LUMP_OFFS

    fwrite(&header, sizeof(header), 1, ptr);
    fwrite(&info, sizeof(bspinfo_t), 1, ptr);
    fwrite(models.data(), sizeof(model_t), models.size(), ptr);
    fwrite(cnodes.data(), sizeof(model_t), models.size(), ptr);
    fwrite(cleaves.data(), sizeof(model_t), models.size(), ptr);
    fwrite(mportals.data(), sizeof(model_t), models.size(), ptr);
    fwrite(portals.data(), sizeof(model_t), models.size(), ptr);
    fwrite(rnodes.data(), sizeof(model_t), models.size(), ptr);
    fwrite(rleafs.data(), sizeof(model_t), models.size(), ptr);
    fwrite(mrsurfs.data(), sizeof(model_t), models.size(), ptr);
    fwrite(rsurfs.data(), sizeof(model_t), models.size(), ptr);
    fwrite(mverts.data(), sizeof(model_t), models.size(), ptr);
    fwrite(verts.data(), sizeof(model_t), models.size(), ptr);
    fwrite(entstring.data(), 1, entstring.size(), ptr);
    fwrite(&nullterm, 1, 1, ptr);

    fclose(ptr);
}

void Bsplib::BspFile::Read(const char* path)
{
    UTILS_ASSERT(path);

    // TODO: Read
    UTILS_ASSERT(0);
}