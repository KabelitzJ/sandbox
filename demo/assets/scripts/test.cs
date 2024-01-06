public class ThirdPersonCamera : MonoBehaviour
{
    public Transform target; // The target GameObject to follow
    public float minDistance = 2.0f; // Minimum distance of the camera from the target
    public float maxDistance = 10.0f; // Maximum distance of the camera from the target
    public float rotationSpeed = 2.0f; // Rotation speed of the camera
    public float zoomSpeed = 2.0f; // Zoom speed of the camera
    public float verticalAngle = 45.0f; // Vertical angle of the camera

    private float currentRotation = 0.0f;
    private float currentDistance = 5.0f;

    void Update()
    {
        // Move the target forward/backward and sideways based on input
        float horizontalInput = Input.GetAxis("Horizontal");
        float verticalInput = Input.GetAxis("Vertical");
        target.Translate(new Vector3(horizontalInput, 0, verticalInput) * Time.deltaTime * 5.0f);

        // Rotate the camera around the target when Q or E keys are pressed
        if (Input.GetKey(KeyCode.Q))
        {
            currentRotation -= rotationSpeed * Time.deltaTime;
        }
        else if (Input.GetKey(KeyCode.E))
        {
            currentRotation += rotationSpeed * Time.deltaTime;
        }

        // Zoom in and out using the scroll wheel
        float scroll = Input.GetAxis("Mouse ScrollWheel");
        currentDistance = Mathf.Clamp(currentDistance - scroll * zoomSpeed, minDistance, maxDistance);

        // Calculate camera position based on rotation, distance, and height
        float horizontalAngle = currentRotation * Mathf.Deg2Rad;
        float verticalAngleRad = verticalAngle * Mathf.Deg2Rad;
        float xPos = target.position.x + currentDistance * Mathf.Sin(horizontalAngle) * Mathf.Cos(verticalAngleRad);
        float yPos = target.position.y + currentDistance * Mathf.Tan(verticalAngleRad);
        float zPos = target.position.z + currentDistance * Mathf.Cos(horizontalAngle) * Mathf.Cos(verticalAngleRad);

        // Set the camera's position and look at the target
        transform.position = new Vector3(xPos, yPos, zPos);
        transform.LookAt(target);
    }
}
