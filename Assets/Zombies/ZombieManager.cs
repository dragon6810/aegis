using System;
using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;

[Serializable]
class ZombieWave
{
    public int numZombies;
    public float time;
}

public class ZombieManager : MonoBehaviour
{
    [SerializeField] GameObject zombie;
    [SerializeField] List<ZombieWave> waves;
    int currentWave = -1;
    int nextWave = 0;
    float currentWaveTime;

    int waveZombiesSpawned;
    float accumulatedZombies;

    const float minWidth = 180.0F;
    const float maxWidth = 210.0F;

    [DoNotSerialize] public List<Transform> activeZombies = new List<Transform>();
    List<Transform> queuedDeaths = new List<Transform>();

    float spawningFunction(float x, int waveNumber)
    {
        const float e = 2.718281828459045F;
        float z = waves[waveNumber].numZombies;
        float c = waves[waveNumber].time;

        if(x < 0.0F || x > c)
            return 0.0F;

        // Negative exponential from 0 to c tuned so integral is z
        return z * (e - 1.0F) / c * (1 - (x / c)) * Mathf.Exp(-x / c);
    }

    void Start()
    {
        startWave();
    }

    void startWave(int waveNum)
    {
        currentWave = waveNum;
        nextWave = waveNum + 1;

        currentWaveTime = 0.0F;
        waveZombiesSpawned = 0;
        accumulatedZombies = 0.0F;
    }

    void startWave()
    {
        startWave(nextWave);
    }

    void endWave()
    {
        int missing = Mathf.Max(0, waves[currentWave].numZombies - waveZombiesSpawned);

        if(missing > 0)
            spawnZombie(missing);

        currentWave = -1;
    }

    void Update()
    {
        carryOutDeaths();

        if((currentWave < 0) || (currentWave >= waves.Count))
            return;

        if(currentWaveTime >= waves[currentWave].time)
        {
            endWave();
            return;
        }

        float numZombies = spawningFunction(currentWaveTime, currentWave) * Time.deltaTime;
        accumulatedZombies += numZombies;

        int intZombies = Mathf.FloorToInt(accumulatedZombies);
        accumulatedZombies -= (float) intZombies;
        waveZombiesSpawned += intZombies;

        spawnZombie(intZombies);

        currentWaveTime += Time.deltaTime;
    }

    void spawnZombie(int count)
    {
        for(int i = 0; i < count; i++)
            spawnZombie();
    }

    void spawnZombie()
    {
        Vector3 position = new Vector3(0.0F, 10.0F, 0.0F);

        float angle = UnityEngine.Random.Range(0.0F, 2.0F * Mathf.PI);
        float distance = UnityEngine.Random.Range(minWidth, maxWidth);

        position.x = Mathf.Cos(angle) * distance;
        position.z = Mathf.Sin(angle) * distance;

        GameObject zObj = Instantiate(zombie, position, Quaternion.identity, transform);
        activeZombies.Add(zObj.transform);
    }

    void carryOutDeaths()
    {
        if(queuedDeaths.Count == 0)
            return;

        foreach(Transform zObj in queuedDeaths)
        {
            activeZombies.Remove(zObj);
            Destroy(zObj.gameObject);
        }

        queuedDeaths.Clear();
    }

    public void killZombie(Transform zObj)
    {
        queuedDeaths.Add(zObj);
    }
}
