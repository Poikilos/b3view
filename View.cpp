#include "Engine.h"
#include "Utility.h"
#include "View.h"

#include <iostream>

using namespace irr;
using namespace irr::core;
using namespace irr::scene;

void View::setNewCameraPosition()
{
    setNewCameraPosition(m_zUp);
}

void View::setNewCameraPosition(bool zUp)
{
    vector3d<f32> newCameraPosition;
    ICameraSceneNode* camera = m_Engine->m_Scene->getActiveCamera();

    if (zUp) {
        camera->setUpVector(vector3df(0, 0, 1));
    } else {
        camera->setUpVector(vector3df(0, 1, 0));
    }

    vector3df oldCamPos = camera->getPosition();
    // vector3df oldCamRot = camera->getRotation();
    // NOTE: rotationToDirection converts a rotation to a vec3 direction.
    // vector3df oldCamRot = m_Engine->tmpPosVec3f.?(m_Engine->tmpTargetVec3f);
    matrix4 yawMatrix;
    if (m_zUp != zUp) {
        float z;
        z = oldCamPos.Y;
        oldCamPos.Y = oldCamPos.Z;
        oldCamPos.Z = z;
        z = m_Engine->m_CamPos.Y;
        m_Engine->m_CamPos.Y = m_Engine->m_CamPos.Z;
        m_Engine->m_CamPos.Z = z;
        z = m_Engine->m_CamTarget.Y;
        m_Engine->m_CamTarget.Y = m_Engine->m_CamTarget.Z;
        m_Engine->m_CamTarget.Z = z;
        camera->setPosition(m_Engine->m_CamPos);
        camera->setTarget(m_Engine->m_CamTarget);
    }
    if (zUp) {
        newCameraPosition.X = 0;
        newCameraPosition.Y = m_CameraDistance * cos(m_PitchFromTarget);
        newCameraPosition.Z = m_CameraDistance * sin(m_PitchFromTarget);
        yawMatrix.setRotationRadians(vector3df(0, 0, m_YawFromTarget));
    } else {
        newCameraPosition.X = 0;
        newCameraPosition.Y = m_CameraDistance * sin(m_PitchFromTarget);
        newCameraPosition.Z = m_CameraDistance * cos(m_PitchFromTarget);
        yawMatrix.setRotationRadians(vector3df(0, m_YawFromTarget, 0));
    }
    yawMatrix.transformVect(newCameraPosition);

    if (zUp) {
        //camera->setUpVector(vector3df(0, 0, 1));
        //newCameraPosition.Z = oldCamPos.Z;
    } else {
        //camera->setUpVector(vector3df(0, 1, 0));
        //newCameraPosition.Y = oldCamPos.Y;
    }
    camera->setPosition(newCameraPosition);
    // vector3df newRotation();
    // camera->setRotation();
    camera->setTarget(m_Engine->m_CamTarget);

    // Set Light direction
    setNewLightDirection(newCameraPosition, m_Engine->m_CamTarget);
    m_zUp = zUp;
    // std::wcerr << L"  setCameraPosition pitch: " << m_Pitch << endl;
}

void View::setNewLightDirection(const vector3df& cameraPosition, const vector3df& cameraTarget)
{
    ILightSceneNode* light = static_cast<ILightSceneNode*>(m_Engine->m_Scene->getSceneNodeFromId(SIID_LIGHT));

    matrix4 m;
    m.buildRotateFromTo(cameraTarget, vector3df(cameraPosition).invert().normalize());

    light->setRotation(m.getRotationDegrees());
}

View::View(Engine* engine)
{
    m_zUp = false;
    m_Engine = engine;
    m_LastMousePosition = new vector2d<int>();
    m_RotMouse = false;
    m_Shift = false;
    this->m_MouseUser = "";
    // m_Pitch = PI;

    // Initial camera values: see Engine::setupScene

    // Set Camera Distance
    // m_CameraDistance = 10;
    // m_CameraDistance = engine->m_CamTarget.getDistanceFrom(engine->m_CamPos);
    // Calculate offsetVec3 manually, since the object is needed later
    // (Vectors for angle are opposite, since camera revolves around center):
    vector3df offsetVec3(
        engine->m_CamPos.X - engine->m_CamTarget.X,
        engine->m_CamPos.Y - engine->m_CamTarget.Y,
        engine->m_CamPos.Z - engine->m_CamTarget.Z);
    m_CameraDistance = offsetVec3.getLength();

    // NOTE: rotationToDirection converts a rotation to a vec3 direction
    // vector3df rotationVec3 = engine->tmpPosVec3f.?(engine->tmpTargetVec3f);
    // vector3df rotationVec3 = engine->tmpTargetVec3f.?(engine->tmpPosVec3f);

    // see rogerborg on <http://irrlicht.sourceforge.net/forum/viewtopic.php?f=1&t=30477>
    // const f32 dot = engine->tmpTargetVec3f.dotProduct(engine->tmpPosVec3f); // to...(from) // angle only

    m_YawFromTarget = atan2(offsetVec3.X, offsetVec3.Z);
    m_PitchFromTarget = asin(offsetVec3.Y);

    // m_Yaw = rotationVec3.Y;
    // m_Pitch = rotationVec3.X;

    debug() << "STARTING Yaw: " << radToDeg(m_YawFromTarget)<< " Pitch: " << radToDeg(m_PitchFromTarget) << endl;
    setNewCameraPosition();
}

View::~View()
{
    delete m_LastMousePosition;
}

void View::setZUp(bool zUp)
{
    if (zUp != m_zUp) {
        setNewCameraPosition(zUp);
    }
}

float View::cameraDistance()
{
    return m_CameraDistance;
}

void View::setCameraDistance(float cameraDistance)
{
    m_CameraDistance = cameraDistance;
}

bool View::zUp()
{
    return this->m_zUp;
}

// IEventReceiver
bool View::OnEvent(const SEvent& event)
{
    // If it's not a mouse event or window resize event, return
    if (event.EventType != EET_MOUSE_INPUT_EVENT
            && !(event.EventType == EET_USER_EVENT
                 && event.UserEvent.UserData1 == UEI_WINDOWSIZECHANGED)) {
        return false;
    }

    // Handle window resize
    if (event.EventType == EET_USER_EVENT
            && event.UserEvent.UserData1 == UEI_WINDOWSIZECHANGED) {
        dimension2d<u32> windowSize = m_Engine->m_Driver->getScreenSize();
        f32 aspectRatio = static_cast<f32>(windowSize.Width) / static_cast<f32>(windowSize.Height);
        // debug() << "Setting aspect to: " << aspectRatio << endl;
        m_Engine->m_Scene->getActiveCamera()->setAspectRatio(aspectRatio);
    }

    // Handle mouse event
    const SEvent::SMouseInput* mouseEvent = &(event.MouseInput);
    if (this->m_MouseUser != "") {
        // std::cerr << "[View] The mouse is being used by " << this->m_MouseUser << std::endl;
        // Do not allow dragging or scroll wheel movement (or anything else
        // that may be implemented) to affect the view while the user is using
        // a panel or dialog.
        return false;
    }
    if (mouseEvent->Event == EMIE_MMOUSE_PRESSED_DOWN) {
        m_RotMouse = true;
        m_LastMousePosition->X = mouseEvent->X;
        m_LastMousePosition->Y = mouseEvent->Y;
    } else if (mouseEvent->Event == EMIE_MMOUSE_LEFT_UP) {
        m_RotMouse = false;
    } else if (mouseEvent->Event == EMIE_MOUSE_WHEEL) {
        // Zoom camera.
        // debug() << "Wheel=" << mouseEvent->Wheel; // -1 or 1
        vector3df offsetVec3(
            m_Engine->m_CamPos.X - m_Engine->m_CamTarget.X,
            m_Engine->m_CamPos.Y - m_Engine->m_CamTarget.Y,
            m_Engine->m_CamPos.Z - m_Engine->m_CamTarget.Z
        );

        f32 distanceDelta = mouseEvent->Wheel * -1 * (offsetVec3.getLength() / 10);
        ICameraSceneNode* camera = m_Engine->m_Scene->getActiveCamera();
        f32 distanceFactor = distanceDelta / Utility::distance(m_Engine->m_CamPos, m_Engine->m_CamTarget);
        //// m_Engine->m_CamPos.interpolate(m_Engine->m_CamPos, m_Engine->m_CamTarget, distanceFactor);
        //// offsetVec3.normalize();
        // m_Engine->m_CamPos.X -= offsetVec3.X * distanceFactor;
        // m_Engine->m_CamPos.Y -= offsetVec3.Y * distanceFactor;
        // m_Engine->m_CamPos.Z -= offsetVec3.Z * distanceFactor;
        // m_Engine->m_CamTarget.X -= offsetVec3.X * distanceFactor;
        // m_Engine->m_CamTarget.Y -= offsetVec3.Y * distanceFactor;
        // m_Engine->m_CamTarget.Z -= offsetVec3.Z * distanceFactor;

        offsetVec3 = vector3df(
                    m_Engine->m_CamPos.X - m_Engine->m_CamTarget.X,
                    m_Engine->m_CamPos.Y - m_Engine->m_CamTarget.Y,
                    m_Engine->m_CamPos.Z - m_Engine->m_CamTarget.Z
        );
        // camera->setPosition(m_Engine->m_CamPos);
        // camera->setTarget(m_Engine->m_CamTarget);

        // m_Yaw = atan2(offsetVec3.X, offsetVec3.Z);
        // m_Pitch = asin(offsetVec3.Y);

        // m_CameraDistance = Utility::distance(m_Engine->m_CamPos, m_Engine->m_CamTarget);
        if (m_CameraDistance + distanceDelta > 0.0001f) {
            m_CameraDistance += distanceDelta;
        }
        else {
            m_CameraDistance /= 2;
        }
        setNewCameraPosition();
        debug() << "View got the event and used event.MouseInput."
                // << " event.GUIEvent.Caller: " << callerID // not avail in Irrlicht (Use this->m_MouseUser instead)
                << ", m_CamPos: " << m_Engine->m_CamPos.X
                << "," << m_Engine->m_CamPos.Y
                << " m_CamTarget: " << m_Engine->m_CamTarget.X
                << "," << m_Engine->m_CamTarget.Y << endl;
    } else if (m_RotMouse) {
        // debug() << "Yaw: " << radToDeg(m_Yaw) << " Pitch: " << radToDeg(m_Pitch) << endl;
        int dx = mouseEvent->X - m_LastMousePosition->X;
        int dy = mouseEvent->Y - m_LastMousePosition->Y;

        m_LastMousePosition->X = mouseEvent->X;
        m_LastMousePosition->Y = mouseEvent->Y;
        if (m_Engine->KeyIsDown[irr::KEY_RSHIFT] || m_Engine->KeyIsDown[irr::KEY_LSHIFT]) {
            // Pan camera.
            float yDelta = (dy / static_cast<float>(m_Engine->m_Driver->getScreenSize().Height/2)) * m_CameraDistance;
            ICameraSceneNode* camera = m_Engine->m_Scene->getActiveCamera();
            f32 sidewaysAngle = 0;
            vector3df target3df = camera->getTarget();
            vector3df camera3df = camera->getAbsolutePosition();
            vector2df cameraFromTarget;
            cameraFromTarget.X = camera3df.X - target3df.X;
            cameraFromTarget.Y = camera3df.Y - target3df.Y;
            if (!m_zUp) {
                // sidewaysAngle = camera->getRotation().Y;
                cameraFromTarget.Y = camera3df.Z - camera3df.Z;
            }
            // sidewaysAngle = camera->getRotation().Z; // always zero for unknown reason
            // sidewaysAngle = atan2(cameraFromTarget.Y, cameraFromTarget.X);
            // if (sidewaysAngle > 2 * PI)
            //     sidewaysAngle -= 2 * PI;
            // vector3df rotationVec3;
            // rotationVec3 = camera->getRotation();
            vector3df forwards(0, 0, 1);
            vector3df dirVec3 = camera->getUpVector();
            // vector3df camRot = camera->getRotation();
            vector3df camRot(0, 0, 0);
            camRot.X = m_PitchFromTarget;
            if (m_zUp) {
                camRot.Z = m_YawFromTarget;
                forwards = vector3df(0, 1, 0);
                dirVec3.rotateYZBy(camRot.X);
                dirVec3.rotateXYBy(camRot.Z);
            } else {
                camRot.Z = m_PitchFromTarget;
                dirVec3.rotateYZBy(camRot.X);
                dirVec3.rotateXYBy(camRot.Y);
            }
            // vector3df dirVec3 = rotationVec3.rotationToDirection(forwards);
            // // move up and down, not in and out:
            // float z = dirVec3.Z;
            // dirVec3.Z = dirVec3.Y;
            // dirVec3.Z = z;
            dirVec3.X *= yDelta;
            dirVec3.Y *= yDelta;
            dirVec3.Z *= yDelta;
            m_Engine->m_CamPos.X += dirVec3.X;
            m_Engine->m_CamPos.Y += dirVec3.Y;
            m_Engine->m_CamPos.Z += dirVec3.Z;
            m_Engine->m_CamTarget.X += dirVec3.X;
            m_Engine->m_CamTarget.Y += dirVec3.Y;
            m_Engine->m_CamTarget.Z += dirVec3.Z;


            if (m_zUp) {
                // m_Engine->m_CamPos.Z += yDelta;
                // m_Engine->m_CamTarget.Z += yDelta;
            } else {
                // m_Engine->m_CamPos.Y += yDelta;
                // m_Engine->m_CamTarget.Y += yDelta;
            }
            // camera->setPosition(m_Engine->m_CamPos);
            // camera->setTarget(m_Engine->m_CamTarget);
            vector3df offsetVec3(
                m_Engine->m_CamPos.X - m_Engine->m_CamTarget.X,
                m_Engine->m_CamPos.Y - m_Engine->m_CamTarget.Y,
                m_Engine->m_CamPos.Z - m_Engine->m_CamTarget.Z
            );

            // m_CameraDistance = offsetVec3.getLength(); //bad: resets zoom

            // m_Yaw = atan2(offsetVec3.X, offsetVec3.Z);
            // m_Pitch = asin(offsetVec3.Y);
            setNewCameraPosition();
            debug() << "AFTER Yaw: " << radToDeg(m_YawFromTarget)
                    << " Pitch: " << radToDeg(m_PitchFromTarget) << endl;

            float xDelta = (dx / static_cast<float>(m_Engine->m_Driver->getScreenSize().Width/2)) * m_CameraDistance;

            vector3df sidewaysVec3;
            sidewaysAngle = m_YawFromTarget;
            // sidewaysAngle += ((PI / 2.0) * 1.0);
            if (sidewaysAngle > 2 * PI) {
                sidewaysAngle -= 2 * PI;
            }
            else if (sidewaysAngle < -2 * PI) {
                sidewaysAngle += 2 * PI;
            }
            if (sidewaysAngle > PI) {
                sidewaysAngle = sidewaysAngle - 2 * PI;
            }
            sidewaysVec3.X = xDelta * cos(sidewaysAngle);
            if (m_zUp) {
                sidewaysVec3.Y = xDelta * sin(sidewaysAngle);
                sidewaysVec3.Z = 0;
            }
            else {
                sidewaysVec3.Z = xDelta * sin(sidewaysAngle);
                sidewaysVec3.Y = 0;
            }
            debug() << "sidewaysAngle:" << sidewaysAngle
                    << "  zUp:" << ((m_zUp)?"true":"false") << endl;
            m_Engine->m_CamPos.X += sidewaysVec3.X;
            m_Engine->m_CamPos.Y += sidewaysVec3.Y;
            m_Engine->m_CamPos.Z += sidewaysVec3.Z;
            m_Engine->m_CamTarget.X += sidewaysVec3.X;
            m_Engine->m_CamTarget.Y += sidewaysVec3.Y;
            m_Engine->m_CamTarget.Z += sidewaysVec3.Z;
            setNewCameraPosition();
        } else {
            // Revolve camera around object.
            // increments of 120 pixels * PI are equal to 180 deg (PI radians):
            f32 pitchDelta = dy / 120.0f;
            // (This old code which may make assumptions about view tends to lock on min/max)
            // if ((m_Pitch - pitchDelta > (PI - (PI / 2))) && (m_Pitch - pitchDelta < PI + (PI/2)))
            // m_Pitch -= dy / 120.0f;
            m_PitchFromTarget += pitchDelta;
            float minPitch = -PI / 2.0f + PI / 1000.0f;
            float maxPitch = PI / 2.0f - PI / 1000.0f;
            if (m_PitchFromTarget < minPitch)
                m_PitchFromTarget = minPitch;
            else if (m_PitchFromTarget > maxPitch)
                m_PitchFromTarget = maxPitch;
            // std::wcerr << "pitch = " << m_Pitch << endl;

            m_YawFromTarget += dx / 120.0f;

            // Set Camera to new rotation
            setNewCameraPosition();
        }
    }
    return true;
}
