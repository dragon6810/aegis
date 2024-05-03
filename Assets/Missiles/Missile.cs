using System.Collections;
using System.Collections.Generic;
using Unity.Collections;
using Unity.VisualScripting;
using UnityEngine;

public class Missile : MonoBehaviour
{
    public Vector3 velocity;
    public float explodeDistance;
    public GameObject explosion;

    [SerializeField] float kbStrength = 0.1F;
    [SerializeField] float kbSpread = 10.0F;
    [SerializeField] float maxKb = 50.0F;
    [SerializeField] float hurtStrength = 50F;
    [SerializeField] float hurtSpread = 100.0F;
    [SerializeField] float maxHurt = 250.0F;
    
    [DoNotSerialize]
    public ZombieManager manager;

    Vector3 startingPosition;
    // Start is called before the first frame update
    void Start()
    {
        startingPosition = transform.position;
    }

    // Update is called once per frame
    void Update()
    {
        transform.position += velocity * Time.deltaTime;
        if(Vector3.Distance(transform.position, startingPosition) >= explodeDistance)
            explode();
    }

    void explode()
    {
        Instantiate(explosion, transform.position, Quaternion.identity);

        foreach(Transform z in manager.activeZombies)
        {
            Rigidbody rigidbody = z.gameObject.GetComponent<Rigidbody>();

            float distance = Vector2.Distance(new Vector2(transform.position.x, transform.position.z), new Vector2(rigidbody.position.x, rigidbody.position.z));
            Vector3 direction = new Vector3(rigidbody.position.x, 0.0F, rigidbody.position.z) - new Vector3(transform.position.x, 0.0F, transform.position.z);
            direction = direction.normalized;

            float kb = kbStrength / ((distance / kbSpread) * (distance / kbSpread));
            kb = Mathf.Min(kb, maxKb);

            float dmg = hurtStrength / ((distance / hurtSpread) * (distance / hurtSpread));
            dmg = Mathf.Min(dmg, maxHurt);

            rigidbody.AddForce(direction * kb);
            if(dmg >= 1.0F)
                z.gameObject.GetComponent<ZombieAI>().damage(dmg);
        }

        Destroy(gameObject);
    }
}
