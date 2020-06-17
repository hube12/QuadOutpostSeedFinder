#include "finders.h"
#include "generator.h"
#include "layers.h"
#include <unistd.h>

FILE *file;


int testOutpost(Pos *pos, int64_t seed, LayerStack *g) {
    int chunkX=pos->x>>4u;
    int chunkZ=pos->z>>4u;
    int sX = chunkX >> 4u;
    int sZ = chunkZ >> 4u;
    int64_t current_seed = (sX ^ sZ << 4u) ^seed;
    setSeed(&current_seed);
    next(&current_seed, 32);
    if (nextInt(&current_seed, 5) != 0)return 0;
    for (int x = chunkX - 10; x <= chunkX + 10; ++x) {
        for (int z = chunkZ - 10; z <= chunkZ + 10; ++z) {
            Pos chunkVillage = getStructurePos(VILLAGE_CONFIG, seed, x / 32, z / 32);
            if ((chunkVillage.x>>4) == x && (chunkVillage.z >> 4) == z){
                if(isViableVillagePos(*g, NULL, (x << 4) + 9, (z << 4) + 9))return 0;
            }
        }
    }
    return 1;
}

void test(){
    StructureConfig featureConfig=OUTPOST_CONFIG;
    int regPosX = 1;
    int regPosZ = 2;
    Pos qhpos[4]; initBiomes();
    LayerStack g = setupGeneratorMC113();

    int64_t base = 280894333812694L;

    qhpos[0] = getStructurePos(featureConfig, base, 0 + regPosX, 0 + regPosZ);
    if (!testOutpost(&qhpos[0], base, &g)) printf("Fail 1\n");

    qhpos[1] = getStructurePos(featureConfig, base, 0 + regPosX, 1 + regPosZ);
    if (!testOutpost(&qhpos[1], base, &g)) printf("Fail 2\n");

    qhpos[2] = getStructurePos(featureConfig, base, 1 + regPosX, 0 + regPosZ);
    if (!testOutpost(&qhpos[2], base, &g)) printf("Fail 3\n");

    qhpos[3] = getStructurePos(featureConfig, base, 1 + regPosX, 1 + regPosZ);
    if (!testOutpost(&qhpos[3], base, &g)) printf("Fail 4\n");
}

int main(int argc, char *argv[]) {
    file = fopen("save.txt", "w");

    initBiomes();
    LayerStack g = setupGeneratorMC113();
    const char *seedFileName;
    StructureConfig featureConfig=OUTPOST_CONFIG;
    seedFileName = "./quadBase_113.txt";
    if (access(seedFileName, F_OK)) {
        printf("Seed base file does not exist: Creating new one.\nThis may take a few minutes...\n");
        int threads = 6;
        int quality = 1;
        search4QuadBases(seedFileName, threads, featureConfig, quality);
    }
    int64_t qhcnt;
    int64_t *qhcandidates = loadSavedSeeds(seedFileName, &qhcnt);

    Pos qhpos[4];
    int64_t hits = 0;
    //int regPosX = 1;
    //int regPosZ = 2;
    //regPosX -= 1;
    //regPosZ -= 1;
    for (int regPosX = -10; regPosX < 10; ++regPosX) {
        for (int regPosZ = -10; regPosZ < 10; ++regPosZ) {
            for (uint64_t i = 0; i < qhcnt; i++) {
                int64_t base = moveStructure(qhcandidates[i], regPosX, regPosZ);

                qhpos[0] = getStructurePos(featureConfig, base, 0 + regPosX, 0 + regPosZ);
                if (!testOutpost(&qhpos[0], base, &g)) continue;

                qhpos[1] = getStructurePos(featureConfig, base, 0 + regPosX, 1 + regPosZ);
                if (!testOutpost(&qhpos[1], base, &g)) continue;

                qhpos[2] = getStructurePos(featureConfig, base, 1 + regPosX, 0 + regPosZ);
                if (!testOutpost(&qhpos[2], base, &g)) continue;

                qhpos[3] = getStructurePos(featureConfig, base, 1 + regPosX, 1 + regPosZ);
                if (!testOutpost(&qhpos[3], base, &g)) continue;

                for (int k = 0; k < 4; ++k) {
                    printf("x: %d z: %d struct seed: %lld\n", qhpos[k].x/16/32, qhpos[k].z/16/32,base);
                }
                for (uint64_t j = 0; j < 0x10000; j++) {
                    int64_t seed = base + (j << 48u);

                    applySeed(&g, seed);
                    if (!isViableFeaturePos(Outpost, g, NULL, qhpos[0].x, qhpos[0].z)) continue;
                    if (!isViableFeaturePos(Outpost, g, NULL, qhpos[1].x, qhpos[1].z)) continue;
                    if (!isViableFeaturePos(Outpost, g, NULL, qhpos[2].x, qhpos[2].z)) continue;
                    if (!isViableFeaturePos(Outpost, g, NULL, qhpos[3].x, qhpos[3].z)) continue;

                    fprintf(file, "%lld\n", seed);
                    printf("%lld\n", seed);
                    if (!(hits % 1000)) {
                        printf("%lld quad outpost hits \n", hits);
                    }
                    hits++;
                }
            }
        }
    }

    freeGenerator(g);

    return 0;
}


