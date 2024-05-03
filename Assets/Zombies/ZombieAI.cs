using System;
using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;

public class ZombieAI : MonoBehaviour
{
    Rigidbody rigidbody;
    Vector3 velocity = Vector3.zero;
    float friction = 1.0F;
    float maxSpeed = 100.0F;
    float accel = 10.0F;

    float turnSpeed = 120.0F;

    float visionRadius = 50.0F;
    float fov = 270.0F;
    public float maxHealth = 100.0F;

    float health;

    void Start()
    {
        rigidbody = this.GetComponent<Rigidbody>();
        health = maxHealth;
    }

    // Update is called once per frame
    void Update()
    {
        velocity = rigidbody.velocity;

        Vector3 wishDir = calculateWishDir();

        transform.rotation = Quaternion.LookRotation(wishDir, Vector3.up);
        
        velocity -= velocity * friction * Time.deltaTime;

        float currentSpeed = Vector3.Dot(velocity, wishDir);
        float addSpeed = Mathf.Clamp(maxSpeed - currentSpeed, 0, accel * Time.deltaTime);

        velocity += addSpeed * wishDir;

        rigidbody.velocity = velocity;
    }

    Vector3 calculateWishDir()
    {
        Transform manager = transform.parent;
        ZombieManager managerScript = manager.gameObject.GetComponent<ZombieManager>();

        Vector3 wishDir = (Vector3.zero - rigidbody.position).normalized; // Gravitate towards origin (Where the tank is)

        if(managerScript == null)
        {
            Debug.LogError("You have a Zombie that is not a child of a valid manager!");
            return wishDir;
        }

        Vector2 pos = new Vector2(transform.position.x, transform.position.z);
        Vector2 viewDir = new Vector2(transform.forward.x, transform.forward.z);
        float viewAngle = Mathf.Atan2(viewDir.y, viewDir.x) * Mathf.Rad2Deg;

        float rotationDelta = 0.0F;

        rotationDelta += Mathf.Atan2(wishDir.z, wishDir.x) * Mathf.Rad2Deg - viewAngle;

        float sqrVision = visionRadius * visionRadius;

        /*
        foreach(Transform zombie in managerScript.activeZombies)
        {
            if(zombie.position == transform.position)
                continue;

            Vector2 zpos = new Vector2(zombie.position.x, zombie.position.z);

            if(Vector2.SqrMagnitude(pos - zpos) > sqrVision) // Zombie is too far away
                continue;

            float apparentAngle = Mathf.Atan2((zpos - pos).normalized.y, (zpos - pos).normalized.x) * Mathf.Rad2Deg - viewAngle;

            if(apparentAngle > fov) // Can't see zombie
                continue;

            rotationDelta += -apparentAngle;
        }
        */

        float moveAngle = viewAngle + Mathf.Clamp(rotationDelta, -turnSpeed * Time.deltaTime, turnSpeed * Time.deltaTime);

        //moveAngle = viewAngle;

        wishDir = new Vector3(Mathf.Cos(moveAngle * Mathf.Deg2Rad), 0.0F, Mathf.Sin(moveAngle * Mathf.Deg2Rad));

        return wishDir.normalized;
    }

    public void damage(float amount)
    {
        health -= amount;

        if(health <= 0.0F)
            kill();

        gameObject.GetComponent<MeshRenderer>().material.color = Color.Lerp(Color.red, Color.green, health / maxHealth);
    }

    void kill()
    {
        Transform manager = transform.parent;
        ZombieManager managerScript = manager.gameObject.GetComponent<ZombieManager>();

        managerScript.killZombie(transform);
    }
}
