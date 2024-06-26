#include "SModel.h"
#include "loadtexture.h"
#include <glm.hpp>
#include <iostream>
#include <gtc/matrix_transform.hpp>
#include "Quaternion.h"
#include "binaryloader.h"
#include <chrono>
#include "AssetManager.h"
#include <math.h>
#include "Game.h"

#include "mathutils.h"

void SModel::Say(std::string waveform)
{
    std::string path = std::string("valve/sound/") + waveform;
    voicechannel = Game::GetGame().GetAudioManager()->PlaySound(path, 10, { pos[0], pos[1], pos[2] });
}

void SModel::Load(const char* modelname)
{
    memset(bonecontrollerindices, -1, sizeof(bonecontrollerindices));

    loadBytes(modelname, (char**) &header);

    for (int i = 0; i < header->numbonecontrollers; i++)
    {
        mstudiobonecontroller_t* controller = (mstudiobonecontroller_t*)((char*)header + header->bonecontrollerindex) + i;
        bonecontrollerindices[controller->bone] = i;
        bonecontrollervalues[i] = controller->rest * DEG2RAD;
    }

    char texturename[256];
    if (header->numtextures == 0)
    {
        strcpy(texturename, modelname);
        strcpy(&texturename[strlen(texturename) - 4], "T.mdl");
        loadBytes(texturename, (char**)&texheader);
    }
    else
    {
        strcpy(texturename, modelname);
        texheader = header;
    }

    textures = (GLuint*) malloc(sizeof(GLuint) * texheader->numtextures);

    mstudiotexture_t* ptex = (mstudiotexture_t*) ((char*)texheader + texheader->textureindex);
    for (int t = 0; t < texheader->numtextures; t++)
    {
        textures[t] = AssetManager::getInst().getTexture(ptex[t].name, header->name);

        char* texdata = nullptr;
        int width = 0, height = 0;
        loadmstudiotexture((char*)texheader, t, TEXTYPE_MSTUDIO, (int**)&(texdata), &width, &height);
        if (textures[t] == UINT32_MAX)
        {
            textures[t] = AssetManager::getInst().setTexture(ptex[t].name, header->name);
            glBindTexture(GL_TEXTURE_2D, textures[t]);

            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

#ifdef TEXTURE_FILTER
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#else
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#endif // TEXTURE_FILTER

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width >> 0, height >> 0, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata);
        }
        free(texdata);
    }

    if (header->numseqgroups > 1)
    {
        for (int i = 1; i < header->numseqgroups; i++)
        {
            char seqgroupname[256];

            strcpy(seqgroupname, modelname);
            sprintf(&seqgroupname[strlen(seqgroupname) - 4], "%02d.mdl", i);

            loadBytes(&seqgroupname[0], (char**)&seqheader[i]);
        }
    }
}

void SModel::SetPosition(float x, float y, float z)
{
    pos[0] = x;
    pos[1] = y;
    pos[2] = z;
}

void SModel::startseq(int seqindex)
{
    curseq = seqindex;
    seqstarttime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void SModel::SetupLighting()
{
    vec3_t start = { pos[0], pos[1], pos[2] };
    lightdir = { 0, 0, -1 }; // Get light from flooat
    vec3_t end = lightdir * 2048.0;
    end = end + start;
    lightcolor = map->LightColor(start, end);

    // Approximate light direction with in which direction does it generally get brighter

    float gradient[4]; // Average brightness around the model
    vec3_t gradcolor;

    start.x -= 16.0;
    start.y -= 16.0;
    end.x -= 16.0;
    end.y -= 16.0;

    gradcolor = map->LightColor(start, end);
    gradient[0] = (gradcolor.x + gradcolor.y + gradcolor.z) / 768.0;

    start.x += 32.0;
    end.x += 32.0;

    gradcolor = map->LightColor(start, end);
    gradient[1] = (gradcolor.x + gradcolor.y + gradcolor.z) / 768.0;

    start.y += 32.0;
    end.y += 32.0;

    gradcolor = map->LightColor(start, end);
    gradient[2] = (gradcolor.x + gradcolor.y + gradcolor.z) / 768.0;

    start.x -= 32.0;
    end.x -= 32.0;

    gradcolor = map->LightColor(start, end);
    gradient[3] = (gradcolor.x + gradcolor.y + gradcolor.z) / 768.0;

    lightdir.x = gradient[0] - gradient[1] - gradient[2] + gradient[3];
    lightdir.y = gradient[1] + gradient[0] - gradient[2] - gradient[3];
    lightdir.z = 0;
    lightdir = NormalizeVector3(lightdir);

    float maxchannel = lightcolor.x;
    if (lightcolor.y > maxchannel) maxchannel = lightcolor.y;
    if (lightcolor.z > maxchannel) maxchannel = lightcolor.z;
    maxchannel /= 255.0;

    lightdir = lightdir * maxchannel;

    ambientlight = 0.6;
    lightdir = lightdir * ambientlight;

    directlight = Vector3Length(lightdir);
    ambientlight = maxchannel - directlight;

    if (maxchannel > 0.0)
    {
        lightcolor.x = lightcolor.x / maxchannel;
        lightcolor.y = lightcolor.y / maxchannel;
        lightcolor.z = lightcolor.z / maxchannel;
    }
    else
        lightcolor.x = lightcolor.y = lightcolor.z = 1.0;

    if (ambientlight > 0.5)
        ambientlight = 0.5;

    if (ambientlight + directlight > 1.0)
        directlight = 1.0 - ambientlight;

    lightdir.z = 1;
    lightdir = NormalizeVector3(lightdir);
}

void SModel::render()
{
    SetupLighting();

    float camerapos[3]{ this->camerapos.x, this->camerapos.y, this->camerapos.z };

    glActiveTexture(GL_TEXTURE0);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_TEXTURE_2D);

    mstudiobodypart_t* bodyparts = (mstudiobodypart_t*)((char*)header + header->bodypartindex);
    mstudiotexture_t* ptextures = (mstudiotexture_t*)((char*)texheader + texheader->textureindex);
    mstudiobone_t* pbones = (mstudiobone_t*)((char*)header + header->boneindex);
    mstudiobonecontroller_t* pbonecontrollers = (mstudiobonecontroller_t*)((char*)header + header->bonecontrollerindex);
       
    for (int b = 0; b < header->numbones; b++)
        boneTransforms[b] = transformfrombone(b);

    for (int b = 0; b < header->numbones; b++)
    {
        if (pbones[b].parent == -1)
            continue;
        boneTransforms[b] = boneTransforms[pbones[b].parent] * boneTransforms[b];
    }

    for (int b = 0; b < header->numbones; b++)
    {
        float up[3] =    { 0.0, 0.0, 1.0 };
        float right[3] = { 1.0, 0.0, 0.0 };

        float bone[3] = { boneTransforms[b].val[0][3], boneTransforms[b].val[1][3], boneTransforms[b].val[2][3] };
        Vector3 bonev = Vector3(bone).normalized();

        Mat3x4 rotonly = boneTransforms[b];
        rotonly.val[0][3] = 0.0;
        rotonly.val[1][3] = 0.0;
        rotonly.val[2][3] = 0.0;

        bonev = rotonly * bonev;

        Vector3 boner = Vector3::cross(Vector3(up), bonev).normalized();
        boneright[b].x = boner.get(0);
        boneright[b].y = boner.get(2);
        boneright[b].z = boner.get(1);

        Vector3 boneu = Vector3::cross(bonev, boner);
        boneup[b].x = boneu.get(0);
        boneup[b].y = boneu.get(2);
        boneup[b].z = boneu.get(1);
    }

    for (int b = 0; b < header->numbodyparts; b++)
    {
        mstudiobodypart_t* pbodypart = &bodyparts[b];
        mstudiomodel_t* pmodel = (mstudiomodel_t*)((char*)header + pbodypart->modelindex);
        vec3_t* pstudioverts = (vec3_t*)((char*)header + pmodel->vertindex);
        vec3_t* pstudionorms = (vec3_t*)((char*)header + pmodel->normindex);
        ubyte_t* pvertbone = (ubyte_t*)((char*)header + pmodel->vertinfoindex);
        ubyte_t* pnormbone = (ubyte_t*)((char*)header + pmodel->norminfoindex);
        
        for (int v = 0; v < pmodel->numverts; v++)
        {
            float coords[3] = { pstudioverts[v].x, pstudioverts[v].y, pstudioverts[v].z };
            Vector3 vec = Vector3(coords);
            vec = boneTransforms[pvertbone[v]] * vec;
            xformverts[v].x = vec.get(0);
            xformverts[v].y = vec.get(1);
            xformverts[v].z = vec.get(2);
        }
        
        for (int n = 0; n < pmodel->numnorms; n++)
        {
            float coords[3] = { pstudionorms[n].x, pstudionorms[n].y, pstudionorms[n].z };
            Vector3 vec = Vector3(coords);
            Mat3x4 rotonly = boneTransforms[pnormbone[n]];
            rotonly.val[0][3] = rotonly.val[1][3] = rotonly.val[2][3] = 0.0;
            vec = rotonly * vec;
            xformnorms[n].x = vec.get(0);
            xformnorms[n].y = vec.get(1);
            xformnorms[n].z = vec.get(2);

            vec3_t norm = { xformnorms[n].x, xformnorms[n].y, xformnorms[n].z };
            float color = DotProduct(lightdir, norm) * directlight;
            if (color < 0.0) color = 0.0;
            color += ambientlight;
            lightvals[n].x = color * (float) lightcolor.x / 255.0;
            lightvals[n].y = color * (float) lightcolor.y / 255.0;
            lightvals[n].z = color * (float) lightcolor.z / 255.0;
        }

        glCullFace(GL_FRONT);

        for (int m = 0; m < pmodel->nummesh; m++)
        {
            mstudiomesh_t* pmesh = (mstudiomesh_t*)((char*)header + pmodel->meshindex) + m;

            short* ptricmds = (short*)((char*)header + pmesh->triindex);

            int texindex = pmesh->skinref;
            glBindTexture(GL_TEXTURE_2D, textures[texindex]);
            
            while (int i = *(ptricmds++))
            {
                if (i < 0)
                {
                    i = -i;
                    glBegin(GL_TRIANGLE_FAN);
                }
                else
                {
                    glBegin(GL_TRIANGLE_STRIP);
                }

                for (; i > 0; i--, ptricmds += 4)
                {
                    vec3_t position = xformverts[ptricmds[0]];
                    vec3_t norm = xformnorms[ptricmds[1]];

                    glColor3f(lightvals[ptricmds[1]].x, lightvals[ptricmds[1]].y, lightvals[ptricmds[1]].z);

                    if (ptextures[pmesh->skinref].flags & STUDIO_NF_CHROME)
                    {
                        float posv[3] = { position.x, position.y, position.z };
                        float normv[3] = { norm.x, norm.y, norm.z };
                        Vector3 toeye = Vector3(Game::GetGame().camera.position) - Vector3(posv);
                        toeye.normalize();
                        Vector3 reflected = Vector3::reflect(toeye, Vector3(normv));
                        float bup[3] = { boneup[pnormbone[ptricmds[1]]].x, boneup[pnormbone[ptricmds[1]]].y, boneup[pnormbone[ptricmds[1]]].z };
                        float bright[3] = { boneright[pnormbone[ptricmds[1]]].x, boneright[pnormbone[ptricmds[1]]].y, boneright[pnormbone[ptricmds[1]]].z };
                        float s = (Vector3::dot(reflected, Vector3(bright)) * ((float) ptextures[texindex].width / 2.0)) + ((float) ptextures[texindex].width / 2.0);
                        float t = (Vector3::dot(reflected, Vector3(bup)) * ((float) ptextures[texindex].height / 2.0)) + ((float) ptextures[texindex].height / 2.0);
                        glTexCoord2f(s / (float)ptextures[texindex].width, t / (float)ptextures[texindex].height);
                    }
                    else
                    {
                        glTexCoord2f((float)ptricmds[2] / (float)ptextures[texindex].width, (float)ptricmds[3] / (float)ptextures[texindex].height);
                    }
                    
                    glVertex3f(position.x, position.y, position.z);
                }
                glEnd();
            }
        }
    }

    glColor3f(1, 1, 1);

    glDisable(GL_TEXTURE_2D);
}

void SModel::RenderHitboxes()
{
    for (int i = 0; i < header->numhitboxes; i++)
    {
        mstudiohitbox_t* hbox = (mstudiohitbox_t*)((char*)header + header->hitboxindex) + i;

        float bbminf[3] = { hbox->bbmin.x, hbox->bbmin.y, hbox->bbmin.z };
        float bbmaxf[3] = { hbox->bbmax.x, hbox->bbmax.y, hbox->bbmax.z };

        Vector3 bbminv = boneTransforms[hbox->bone] * Vector3(bbminf);
        Vector3 bbmaxv = boneTransforms[hbox->bone] * Vector3(bbmaxf);

        float temp[3]{};
        Vector3 corner0 = boneTransforms[hbox->bone] * Vector3(bbminf);
        temp[0] = bbmaxf[0]; temp[1] = bbminf[1]; temp[2] = bbminf[2];
        Vector3 corner1 = boneTransforms[hbox->bone] * Vector3(temp);
        temp[0] = bbmaxf[0]; temp[1] = bbmaxf[1]; temp[2] = bbminf[2];
        Vector3 corner2 = boneTransforms[hbox->bone] * Vector3(temp);
        temp[0] = bbminf[0]; temp[1] = bbmaxf[1]; temp[2] = bbminf[2];
        Vector3 corner3 = boneTransforms[hbox->bone] * Vector3(temp);
        temp[0] = bbminf[0]; temp[1] = bbminf[1]; temp[2] = bbmaxf[2];
        Vector3 corner4 = boneTransforms[hbox->bone] * Vector3(temp);
        temp[0] = bbmaxf[0]; temp[1] = bbminf[1]; temp[2] = bbmaxf[2];
        Vector3 corner5 = boneTransforms[hbox->bone] * Vector3(temp);
        temp[0] = bbmaxf[0]; temp[1] = bbmaxf[1]; temp[2] = bbmaxf[2];
        Vector3 corner6 = boneTransforms[hbox->bone] * Vector3(temp);
        temp[0] = bbminf[0]; temp[1] = bbmaxf[1]; temp[2] = bbmaxf[2];
        Vector3 corner7 = boneTransforms[hbox->bone] * Vector3(temp);

        glColor3f(1, 0, 0);

        glBegin(GL_LINES);

        // Draw bottom face
        glVertex3f(corner0.get(0), corner0.get(1), corner0.get(2)); glVertex3f(corner1.get(0), corner1.get(1), corner1.get(2));
        glVertex3f(corner1.get(0), corner1.get(1), corner1.get(2)); glVertex3f(corner2.get(0), corner2.get(1), corner2.get(2));
        glVertex3f(corner2.get(0), corner2.get(1), corner2.get(2)); glVertex3f(corner3.get(0), corner3.get(1), corner3.get(2));
        glVertex3f(corner3.get(0), corner3.get(1), corner3.get(2)); glVertex3f(corner0.get(0), corner0.get(1), corner0.get(2));

        // Draw top face
        glVertex3f(corner4.get(0), corner4.get(1), corner4.get(2)); glVertex3f(corner5.get(0), corner5.get(1), corner5.get(2));
        glVertex3f(corner5.get(0), corner5.get(1), corner5.get(2)); glVertex3f(corner6.get(0), corner6.get(1), corner6.get(2));
        glVertex3f(corner6.get(0), corner6.get(1), corner6.get(2)); glVertex3f(corner7.get(0), corner7.get(1), corner7.get(2));
        glVertex3f(corner7.get(0), corner7.get(1), corner7.get(2)); glVertex3f(corner4.get(0), corner4.get(1), corner4.get(2));

        // Draw connecting edges
        glVertex3f(corner0.get(0), corner0.get(1), corner0.get(2)); glVertex3f(corner4.get(0), corner4.get(1), corner4.get(2));
        glVertex3f(corner1.get(0), corner1.get(1), corner1.get(2)); glVertex3f(corner5.get(0), corner5.get(1), corner5.get(2));
        glVertex3f(corner2.get(0), corner2.get(1), corner2.get(2)); glVertex3f(corner6.get(0), corner6.get(1), corner6.get(2));
        glVertex3f(corner3.get(0), corner3.get(1), corner3.get(2)); glVertex3f(corner7.get(0), corner7.get(1), corner7.get(2));

        glEnd();

        glColor3f(1, 1, 1);
    }

    glColor3f(0, 1, 0);

    glBegin(GL_LINES);

    // Bottom face
    glVertex3f(header->min.x, header->min.y, header->min.z); glVertex3f(header->max.x, header->min.y, header->min.z);
    glVertex3f(header->max.x, header->min.y, header->min.z); glVertex3f(header->max.x, header->max.y, header->min.z);
    glVertex3f(header->max.x, header->max.y, header->min.z); glVertex3f(header->min.x, header->max.y, header->min.z);
    glVertex3f(header->min.x, header->max.y, header->min.z); glVertex3f(header->min.x, header->min.y, header->min.z);

    // Top face
    glVertex3f(header->min.x, header->min.y, header->max.z); glVertex3f(header->max.x, header->min.y, header->max.z);
    glVertex3f(header->max.x, header->min.y, header->max.z); glVertex3f(header->max.x, header->max.y, header->max.z);
    glVertex3f(header->max.x, header->max.y, header->max.z); glVertex3f(header->min.x, header->max.y, header->max.z);
    glVertex3f(header->min.x, header->max.y, header->max.z); glVertex3f(header->min.x, header->min.y, header->max.z);

    // Connecting edges
    glVertex3f(header->min.x, header->min.y, header->min.z); glVertex3f(header->min.x, header->min.y, header->max.z);
    glVertex3f(header->max.x, header->min.y, header->min.z); glVertex3f(header->max.x, header->min.y, header->max.z);
    glVertex3f(header->max.x, header->max.y, header->min.z); glVertex3f(header->max.x, header->max.y, header->max.z);
    glVertex3f(header->min.x, header->max.y, header->min.z); glVertex3f(header->min.x, header->max.y, header->max.z);

    glEnd();

    glDisable(GL_DEPTH_TEST);

    glPointSize(5.0f);
    glColor3f(1, 1, 0);
    for (int i = 0; i < header->numbones; i++)
    {
        mstudiobone_t* pbones = (mstudiobone_t*)((char*)header + header->boneindex);
        mstudiobone_t* pbone = pbones + i;

        Mat3x4 transform = boneTransforms[i];
        float posf[3] = { transform.val[0][3], transform.val[1][3], transform.val[2][3] };
        Vector3 pos(posf);

        if (pbone->parent >= 0)
        {
            mstudiobone_t* parent = nullptr;
            if (pbone->parent >= 0)
                parent = pbones + pbone->parent;

            transform = boneTransforms[pbone->parent];
            posf[0] = transform.val[0][3]; posf[1] = transform.val[1][3]; posf[2] = transform.val[2][3];
            Vector3 parentv(posf);

            glBegin(GL_LINES);

            glVertex3f(pos.get(0), pos.get(1), pos.get(2));
            glVertex3f(parentv.get(0), parentv.get(1), parentv.get(2));

            glEnd();
        }
    }

    glColor3f(0, 0, 1);
    glBegin(GL_POINTS);
    for (int i = 0; i < header->numbones; i++)
    {
        mstudiobone_t* pbones = (mstudiobone_t*)((char*)header + header->boneindex);
        mstudiobone_t* pbone = pbones + i;

        Mat3x4 transform = boneTransforms[i];
        float posf[3] = { transform.val[0][3], transform.val[1][3], transform.val[2][3] };
        Vector3 pos(posf);

        glVertex3f(pos.get(0), pos.get(1), pos.get(2));
    }
    glEnd();

    glEnable(GL_DEPTH_TEST);

    glColor3f(1, 1, 1);
}

void SModel::Tick()
{
    if (voicechannel >= 0)
    {
        if (!Game::GetGame().GetAudioManager()->GetChannel(voicechannel)->playing)
            voicechannel = -1;
    }

    mstudioseqdescription_t* seq = (mstudioseqdescription_t*)((char*)header + header->seqindex) + curseq;
    int before = (int)frame % (seq->numframes - 1);
    frame += ENGINE_TICKDUR * seq->fps;
    int now = (int)frame % (seq->numframes - 1);

    mstudioanimevent_t* events = (mstudioanimevent_t*)((char*)header + seq->eventindex);
    for (int i = 0; i < seq->numevents; i++)
    {
        mstudioanimevent_t event = events[i];
        if ((now >= event.frame) && before < (event.frame))
        {
            if (event.event == STUDIO_EVENT_SOUND)
            {
                std::string path = std::string("valve/sound/") + std::string(event.options);
                Game::GetGame().GetAudioManager()->PlaySound(path, 5, { pos[0], pos[1], pos[2] });
            }
            else if (event.event == STUDIO_EVENT_SOUND_VOICE)
            {
                std::string path = std::string("valve/sound/") + std::string(event.options);
                voicechannel = Game::GetGame().GetAudioManager()->PlaySound(path, 10, { pos[0], pos[1], pos[2] });
            }
        }
    }

    for (int i = 0; i < header->numbonecontrollers; i++)
    {
        mstudiobonecontroller_t* controller = (mstudiobonecontroller_t*)((char*)header + header->bonecontrollerindex) + i;
        if (controller->index != 4)
            continue;

        if (voicechannel != -1)
        {
            audiochannel_t* voice = Game::GetGame().GetAudioManager()->GetChannel(voicechannel);
            const short* sounddata = voice->sound.sound->getSamples();
            int time = (int)((voice->sound.duration - voice->timeleft) * voice->sound.samplerate);
            float val = sounddata[time] / (float) voice->sound.max * (controller->end - controller->start) + controller->start;

            if (val > controller->end)
                val = controller->end;
            if (val < controller->start)
                val = controller->start;

            bonecontrollervalues[i] = -val * DEG2RAD;
        }
        else
        {
            bonecontrollervalues[i] = -controller->rest * DEG2RAD;
        }
    }

    lastlasttickframe = lasttickframe;
    lasttickframe = frame;

    memcpy(lastlasbonecontrollervalues, lastbonecontrollervalues, sizeof(lastlasbonecontrollervalues));
    memcpy(lastbonecontrollervalues, bonecontrollervalues, sizeof(bonecontrollervalues));
}

Mat3x4 SModel::transformfrombone(int boneindex)
{
    int sequence = curseq;
    float interframe = lastlasttickframe + (lasttickframe - lastlasttickframe) * Game::GetGame().tickinterp;
    int f = (int)interframe;
    float s = interframe - f;

    mstudiobone_t* pbone = (mstudiobone_t*)((char*)header + header->boneindex) + boneindex;

    mstudioseqdescription_t* pseqdesc = (mstudioseqdescription_t*)((char*)header + header->seqindex) + sequence;
    mstudioseqgroup_t* pseqgroup = (mstudioseqgroup_t*)((char*)header + header->seqgroupindex) + pseqdesc->seqgroup;

    f %= pseqdesc->numframes - 1;

    mstudioanimchunk_t* panim;
    if (pseqdesc->seqgroup == 0)
        panim = (mstudioanimchunk_t*)((char*)header + pseqgroup->unused2 + pseqdesc->animindex) + boneindex;
    else
        panim = (mstudioanimchunk_t*)((char*)seqheader[pseqdesc->seqgroup] + pseqdesc->animindex) + boneindex;

    float pos[3];
    Quaternion rot;

    float a1[3];
    float a2[3];
    for (int i = 0; i < 3; i++)
    {
        if (panim->offset[i + 3] == 0)
        {
            a2[i] = a1[i] = pbone->value[i + 3];
        }
        else
        {
            mstudioanimvalue_t* panimval = (mstudioanimvalue_t*)((char*)panim + panim->offset[i + 3]);
            int j = f;
            while (panimval->num.total <= j)
            {
                j -= panimval->num.total;
                panimval += panimval->num.valid + 1;
            }

            if (panimval->num.valid > j)
            {
                a1[i] = panimval[j + 1].value;

                if (panimval->num.valid > j + 1)
                {
                    a2[i] = panimval[j + 2].value;
                }
                else
                {
                    if (panimval->num.total > j + 1)
                        a2[i] = a1[i];
                    else
                        a2[i] = panimval[panimval->num.valid + 2].value;
                }
            }
            else
            {
                a1[i] = panimval[panimval->num.valid].value;
                if (panimval->num.total > j + 1)
                    a2[i] = a1[i];
                else
                    a2[i] = panimval[panimval->num.valid + 2].value;
            }

            a1[i] = pbone->value[i + 3] + a1[i] * pbone->scale[i + 3];
            a2[i] = pbone->value[i + 3] + a2[i] * pbone->scale[i + 3];
        }

        if (pbone->bonecontroller[i + 3] != -1)
        {
            //a1[i] += adj[pbone->bonecontroller[i + 3]];
        }
    }

    if (a1 == a2)
    {
        rot = Quaternion::FromAngle(a1);
    }
    else
    {
        Quaternion q1 = Quaternion::FromAngle(a1);
        Quaternion q2 = Quaternion::FromAngle(a2);
        rot = Quaternion::Slerp(q1, q2, s);
    }

    for (int i = 0; i < 3; i++)
    {
        pos[i] = pbone->value[i];
        if(panim->offset[i] != 0)
        {
            mstudioanimvalue_t* panimval = (mstudioanimvalue_t*)((char*)panim + panim->offset[i]);

            int j = f;
            while (panimval->num.total <= j)
            {
                j -= panimval->num.total;
                panimval += panimval->num.valid + 1;
            }

            if (panimval->num.valid > j)
            {
                if (panimval->num.valid > j + 1)
                    pos[i] += (panimval[j + 1].value * (1.0F - s) + s * panimval[j + 2].value) * pbone->scale[i];
                else
                    pos[i] += panimval[j + 1].value * pbone->scale[i];
            }
            else
            {
                if (panimval->num.total <= j + 1)
                    pos[i] += (panimval[panimval->num.valid].value * (1.0F - s) + s * panimval[panimval->num.valid + 2].value) * pbone->scale[i];
                else
                    pos[i] += panimval[panimval->num.valid].value * pbone->scale[i];
            }
        }
    }

    if (pseqdesc->motionbone == boneindex)
    {
        if (pseqdesc->motiontype & STUDIO_X)
            pos[0] = 0.0F;
        if (pseqdesc->motiontype & STUDIO_Y)
            pos[1] = 0.0F;
        if (pseqdesc->motiontype & STUDIO_Z)
            pos[2] = 0.0F;
    }

    if (bonecontrollerindices[boneindex] >= 0)
    {
        mstudiobonecontroller_t* bonecontroller = (mstudiobonecontroller_t*)((char*)header + header->bonecontrollerindex) + bonecontrollerindices[boneindex];
        float val = lastlasbonecontrollervalues[bonecontrollerindices[boneindex]] + 
            (lastbonecontrollervalues[bonecontrollerindices[boneindex]] - lastlasbonecontrollervalues[bonecontrollerindices[boneindex]]) * s;
        switch (bonecontroller->type & STUDIO_FLAGS_TYPES)
        {
        case STUDIO_FLAGS_X:
            pos[0] += val;
            break;
        case STUDIO_FLAGS_Y:
            pos[1] += val;
            break;
        case STUDIO_FLAGS_Z:
            pos[2] += val;
            break;
        case STUDIO_FLAGS_XR:
            rot = rot * Quaternion::FromAngle({ val, 0, 0 });
            break;
        case STUDIO_FLAGS_YR:
            rot = rot * Quaternion::FromAngle({ 0, val, 0 });
            break;
        case STUDIO_FLAGS_ZR:
            rot = rot * Quaternion::FromAngle({ 0, 0, val });
            break;
        default:
            break;
        }
    }

    Mat3x4 rotationMat = rot.toMat();

    rotationMat.val[0][3] = pos[0];
    rotationMat.val[1][3] = pos[1];
    rotationMat.val[2][3] = pos[2];
    
    return rotationMat;
}

SModel::~SModel()
{

}

void SModel::Cleanup()
{
    for (int s = 1; s < header->numseqgroups; s++)
        free(seqheader[s]);

    free(textures);
}