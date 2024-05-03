using System.Collections;
using System.Collections.Generic;
using Unity.Collections;
using Unity.VisualScripting;
using UnityEditor;
using UnityEngine;
using UnityEngine.Analytics;
using UnityEngine.InputSystem;

public class Fire : MonoBehaviour
{
    [SerializeField] float recoilTime = 0.05F;
    [SerializeField] float recoilDepth = -15.0F;
    [SerializeField] float animationDuration = 2.0F;
    [SerializeField] AudioSource fireSound;
    [SerializeField] GameObject shockwavePrefab;
    [SerializeField] float shockwaveOffset;
    [SerializeField] bool shootMissiles = false;
    [SerializeField] float missileSpeed = 10;
    [SerializeField] GameObject missilePrefab;
    [SerializeField] GameObject zombieManager;
    Vector3 originalPos;
    float animationTime;
    PlayerActions playerControls;
    InputAction fire;

    void Awake()
    {
        playerControls = new PlayerActions();
    }

    void OnEnable()
    {
        fire = playerControls.Input.Fire;
        fire.Enable();
    }

    void OnDisable()
    {
        fire.Disable();
    }

    void Start()
    {
        originalPos = transform.localPosition;
        animationTime = animationDuration;
    }

    IEnumerator LandMissile(Vector3 position)
    {
        yield return new WaitForSeconds(Random.Range(0.0F, 0.1F));

        Instantiate(shockwavePrefab, position, Quaternion.identity);
    }

    void Shoot()
    {
        fireSound.Play(0);
        Instantiate(shockwavePrefab, transform.position + transform.up * shockwaveOffset + Vector3.up * 0.1F, Quaternion.identity);

        Vector3 vel = transform.up * missileSpeed;
        Vector3 pos = transform.position + transform.up * shockwaveOffset;
        Vector3 endPos = new Vector3(LookAtMouse.GetGameMousePosition().x, transform.position.y, LookAtMouse.GetGameMousePosition().y);

        GameObject missileObj = Instantiate(missilePrefab, pos, Quaternion.identity);
        var missileScript = missileObj.GetComponent<Missile>();
        missileScript.velocity = vel;
        missileScript.explodeDistance = Vector3.Distance(endPos, pos);
        missileScript.manager = zombieManager.GetComponent<ZombieManager>();
        
        if(LookAtMouse.GetGameMousePosition().magnitude < new Vector2(pos.x, pos.z).magnitude)
            missileScript.explodeDistance = 0.0F;
    }

    void Update()
    {
        animationTime += Time.deltaTime;
        if(animationTime > animationDuration)
        {
            transform.localPosition = originalPos;
            animationTime = animationDuration;
        }
        else
        {
            transform.localPosition = originalPos;
            transform.Translate(transform.up * recoilFunction(animationTime / animationDuration), Space.World);
        }

        if(fire.ReadValue<float>() > 0.0F && animationTime >= animationDuration)
        {
            if(shootMissiles)
            {
                Shoot();
            }
            animationTime = 0.0F;
        }
    }

    float Remap(float val, float from1, float to1, float from2, float to2) 
    {
        return (val - from1) / (to1 - from1) * (to2 - from2) + from2;
    }

    float recoilFunction(float t)
    {
        float recoil = -recoilDepth / (recoilTime * recoilTime) * ((t - recoilTime) * (t - recoilTime)) + recoilDepth;
        if(t <= recoilTime)
            return recoil;

        float returnX = Remap(t, 0.0F, 1.0F, recoilTime, 1.0F);
        float returnVal = returnX * returnX / (2 * (returnX * returnX - returnX) + 1);
        returnVal = Remap(returnVal, 0.0F, 1.0F, recoilDepth, 0.0F);
        return returnVal;
    }
}
