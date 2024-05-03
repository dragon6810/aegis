using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;

public class ExplosionAudio : MonoBehaviour
{
    public AudioClip explode1;
    public AudioClip explode2;
    public AudioClip explode3;
    // Start is called before the first frame update

    float secondsAlive = 0.0F;
    void Start()
    {
        AudioClip clip;
        int rand = new System.Random().Next(0, 2);
        clip = rand == 0 ? explode1 : rand == 1 ? explode2 : explode3;
        this.GetComponent<AudioSource>().clip = clip;
        this.GetComponent<AudioSource>().Play();
    }

    // Update is called once per frame
    void Update()
    {
        if(!this.GetComponent<AudioSource>().isPlaying)
            Destroy(gameObject);
        secondsAlive += Time.deltaTime;
    }
}
