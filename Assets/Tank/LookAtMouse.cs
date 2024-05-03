using System.Collections;
using System.Collections.Generic;
using Unity.Mathematics;
using UnityEngine;

public class LookAtMouse : MonoBehaviour
{
    void Update()
    {
        Camera cam = Camera.main;
        Vector2 mousePos = GetGameMousePosition();
        Vector2 dir = mousePos - new Vector2(transform.position.x, transform.position.z);
        float angle = Mathf.Atan2(dir.y, dir.x) * Mathf.Rad2Deg;
        transform.rotation = Quaternion.AngleAxis(-angle - 90.0F, Vector3.up) * Quaternion.AngleAxis(270.0F, Vector3.right);
    }

    private static float remap(float input, float inputMin, float inputMax, float min, float max)
    {
        return min + (input - inputMin) * (max - min) / (inputMax - inputMin);
    }

    public static Vector2 GetGameMousePosition()
    {
        float mainAspect = Camera.main.aspect;
        float screenAspect = (float) Screen.width / (float) Screen.height;

        Vector2 mousePosition = Input.mousePosition;
        mousePosition.x /= (float) Screen.width;
        mousePosition.y /= (float) Screen.height;

        Vector2 gameArea;
        Vector2 offset = Vector2.zero;
        if(screenAspect > mainAspect)
        {
            gameArea.x = 1.0F / mainAspect;
            gameArea.y = 1.0F;
            offset.x -= (Screen.width - Screen.height * mainAspect) / (2.0F * Screen.width);
        }
        else
        {
            gameArea.x = 1.0F;
            gameArea.y = mainAspect;
        }

        mousePosition += offset;

        mousePosition.x *= 640.0F;
        mousePosition.y *= 480.0F;

        mousePosition /= gameArea;

        Vector3 o = Camera.main.ScreenToWorldPoint(mousePosition);
        Vector3 dir = Camera.main.transform.forward;
        Vector3 pos = dir * (-o.y / dir.y) + o;
        //Debug.Log(dir);

        return new Vector2(pos.x, pos.z);
    }
}
