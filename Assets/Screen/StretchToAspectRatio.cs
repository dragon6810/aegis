using UnityEngine;
using UnityEngine.UI;

[RequireComponent(typeof(RawImage))]
public class StretchToAspectRatio : MonoBehaviour
{
    private RawImage rawImage;

    void Awake()
    {
        rawImage = GetComponent<RawImage>();
    }

    void Update()
    {
        StretchImage();
    }

    private void StretchImage()
    {
        float screenAspect = (float)Screen.width / Screen.height;
        float targetAspect = 4.0f / 3.0f;

        if (screenAspect >= targetAspect)
        {
            float scaleFactor = (float) Screen.height / 480.0F;
            rawImage.rectTransform.localScale = new Vector3(scaleFactor, scaleFactor, 1);
        }
        else
        {
            // Screen is taller than target, so stretch horizontally
            float scaleFactor = (float) Screen.width / 640.0F;
            rawImage.rectTransform.localScale = new Vector3(scaleFactor, scaleFactor, 1);
        }
    }
}