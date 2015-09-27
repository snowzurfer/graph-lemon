// Lab3.cpp
#include "Lab3.h"

Lab3::Lab3(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in) : BaseApplication(hinstance, hwnd, screenWidth, screenHeight, in)
{
	// Create Mesh object
	m_Mesh = new SphereMesh(m_Direct3D->GetDevice(), L"../res/DefaultDiffuse.png");

	m_Shader = new LightShader(m_Direct3D->GetDevice(), hwnd);

  m_Light = new Light();
  // Set the light values
  m_Light->SetDiffuseColour(1.f, 1.f, 1.f, 1.f);
  m_Light->SetAmbientColour(0.3f, 0.3f, 0.3f, 1.f);
  m_Light->SetDirection(0.5f, -0.5f, 0.f);
}


Lab3::~Lab3()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	if (m_Mesh)
	{
		delete m_Mesh;
		m_Mesh = 0;
	}

	if (m_Shader)
	{
		delete m_Shader;
		m_Shader = 0;
	}

  if (m_Light) {
    delete m_Light;
    m_Light = nullptr;
  }
}


bool Lab3::Frame()
{
	bool result;

	result = BaseApplication::Frame();
	if (!result)
	{
		return false;
	}

	// Render the graphics.
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool Lab3::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	
	// Clear the scene. (default blue colour)
	m_Direct3D->BeginScene(0.39f, 0.58f, 0.92f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Send geometry data (from mesh)
	m_Mesh->SendData(m_Direct3D->GetDeviceContext());
	// Set shader parameters (matrices and texture)
	m_Shader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, 
    viewMatrix, projectionMatrix, m_Mesh->GetTexture(), m_Light);
	// Render object (combination of mesh geometry and shader process
	m_Shader->Render(m_Direct3D->GetDeviceContext(), m_Mesh->GetIndexCount());

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}
