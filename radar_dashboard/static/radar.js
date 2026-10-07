
console.log("RADAR.JS LOADED")

// ============================================================
// RADAR DASHBOARD
// Receives:
//     {"step":15,"distance":42}
//
// Step range:
// 5  -> 0°
// 25 -> 180°
//
// WebSocket:
//     /ws
// ============================================================


// ------------------------------------------------------------
// Canvas setup
// ------------------------------------------------------------

const canvas = document.getElementById("radarCanvas");
const ctx = canvas.getContext("2d");


// ------------------------------------------------------------
// Dashboard elements
// ------------------------------------------------------------

const stepDisplay = document.getElementById("step");
const angleDisplay = document.getElementById("angle");
const distanceDisplay = document.getElementById("distance");
const statusDisplay = document.getElementById("status");


// ------------------------------------------------------------
// Radar configuration
// ------------------------------------------------------------

const MIN_STEP = 5;
const MAX_STEP = 25;

let currentStep = MIN_STEP;
let currentAngle = 0;
let currentDistance = 0;


// Maximum distance shown by the radar
let maxRange = 200;


// Detected objects
const detections = [];


// ------------------------------------------------------------
// WebSocket
// ------------------------------------------------------------

let socket = null;
let reconnectTimer = null;


function connectWebSocket()
{
    /*
    * window.location.host automatically gives:
    *
    * laptop:
    *localhost:8000
    *
    * other device:
    *Check IP 
    */
    const protocol =
        window.location.protocol === "https:"
        ? "wss:"
        : "ws:";
    
    const websocketURL = `${protocol}//${window.location.host}/ws`;
        console.log("Connecting to:", websocketURL);
        socket = new WebSocket(websocketURL);
        
        socket.onopen = function()
        {
            console.log("WebSocket connected");
            setStatus("Connected");
            if (reconnectTimer !== null)
            {
                clearTimeout(reconnectTimer);
                reconnectTimer = null;
            }
        };
        socket.onmessage = function(event)
        {
            console.log("Received:", event.data);
            try
            {
                const data = JSON.parse(event.data);
                const step = Number(data.step);
                const distance = Number(data.distance)
                if (!Number.isFinite(step) || !Number.isFinite(distance))
                {
                    console.warn(
                        "Invalid radar data:",
                        data
                    );
                    return;
                }
            
                // Update current radar values
                currentStep = step
                currentAngle = stepToAngle(step);
                currentDistance = distance;

                // Update dashboard text
                updateDisplays();
                
                // Store detection
                
                addDetection(
                    currentAngle,
                    currentDistance
                );
                
                // Redraw radar
                drawRadar();
            }
            
            catch(error)
            {
                console.error(
                    "Invalid WebSocket message:",
                    error
                );
            }
        };
        
//--------------------------------------------------------Connection closed--------------------------------------------------------
        socket.onclose = function()
        {
            console.log("WebSocket disconnected");
            setStatus("Disconnected");
            scheduleReconnect();
        };
        socket.onerror = function(error)
        {
            console.error(
                "WebSocket error:",
                error
            );
            setStatus("Connection Error");
        };
}
function scheduleReconnect()
{
    if (reconnectTimer !== null)
    {
        return;
    }
    reconnectTimer = setTimeout
    (
        function()
        {
            reconnectTimer = null;
            connectWebSocket();
            },
            2000
    );
}

function stepToAngle(step)
{
/*
* Step 5  -> 0°
* Step 15 -> 90°
* Step 25 -> 180°
*/

let angle = (step - MIN_STEP) * 180 / (MAX_STEP - MIN_STEP);

// Keep angle inside valid range
angle = Math.max(0, Math.min(180, angle));
return angle;
}

// -------------Update dashboard values--------------

function updateDisplays()
{
    if (stepDisplay)
    {
        stepDisplay.textContent = currentStep;
    }
    if (angleDisplay)
    {
        angleDisplay.textContent =
        `${currentAngle.toFixed(0)}°`;
    }
    if (distanceDisplay)
    {
        distanceDisplay.textContent = `${currentDistance} cm`;
    }
}
//-------------------------Status display-------------------------------------
function setStatus(status)
{
    if (statusDisplay)
    {
        statusDisplay.textContent = status;
    }
}
//----------------------------------Add detection------------------------------------------
function addDetection(angle, distance)
{
    /*
    * Ignore objects outside the selected radar range.
    */
    if (distance < 0 || distance > maxRange)
    {
        return;
    }
    detections.push({
        angle: angle,
        distance: distance
        });
    /*
    * Keep the number of points reasonable.
    * Older detections are removed automatically.
    */
    if (detections.length > 500)
    {
        detections.shift();
    }
}
//-------------Convert polar coordinates to canvas coordinate---------------
function polarToCanvas(angle, distance)
{
    const centerX = canvas.width / 2;
    const centerY = canvas.height - 20;

    /*
    * Convert distance into radar radius.
    */
    const maxRadius =
    Math.min(
    canvas.width / 2 - 30,
    canvas.height - 40
    );
    const radius =
    (distance / maxRange) * maxRadius;

    /*
    * Radar angle:
    *
    * 0°   = right
    * 90°  = up
    * 180° = left
    */
    const radians =
    angle * Math.PI / 180;
    const x = centerX + radius * Math.cos(radians);
    const y = centerY - radius * Math.sin(radians);
    return {
    x: x,
    y: y
    };
}
//------------------Draw radar background---------------------
function drawRadar()
{
    resizeCanvas();
    ctx.clearRect(
        0,
        0,
        canvas.width,
        canvas.height
    );
    const centerX = canvas.width / 2;
    const centerY = canvas.height - 20;
    const maxRadius =
    Math.min(
    canvas.width / 2 - 30,
    canvas.height - 40
    );
//-------------------------Range rings-----------------
    const ringCount = 4;
    for (let i = 1; i <= ringCount; i++)
    {
        const radius = maxRadius * i / ringCount;
        ctx.beginPath();
        ctx.arc
        (
        centerX,
        centerY,
        radius,
        Math.PI,
        2 * Math.PI
        );
        ctx.stroke();
    }
//----------------------Center line-------------------------
    ctx.beginPath();
    
    ctx.moveTo
    (
        centerX - maxRadius,
        centerY
    );
    ctx.lineTo
    (
        centerX + maxRadius,
        centerY
    );
    ctx.stroke();


//-----------------------Angle lines----------------------

    const angles = [0,45,90,135,180];

    angles.forEach
    (
        function(angle)
        {
            const point = polarToCanvas(angle,maxRange);

            ctx.beginPath();

            ctx.moveTo(centerX,centerY);
            ctx.lineTo(point.x,point.y);

            ctx.stroke();
        }
    );

//---------------------------Angle labels--------------------------

    drawAngleLabels
    (
        centerX,
        centerY,
        maxRadius
    );

//------------------------Distance labels-----------------------

    drawRangeLabels
    (
        centerX,
        centerY,
        maxRadius
    );

//----------------------Detection points----------------------

    drawDetections();

//---------------------------Current sweep line-----------------------


    drawSweepLine
    (
        centerX,
        centerY,
        maxRadius
    );
}

//------------------Angle labels----------------------

function drawAngleLabels(centerX, centerY, maxRadius)
{
    const angles = [0, 45, 90, 135, 180];

    angles.forEach
    (
        function(angle)
        {
            const radians = angle * Math.PI / 180;
            const labelRadius = maxRadius + 20;
            const x = centerX + labelRadius * Math.cos(radians);

            const y = centerY - labelRadius * Math.sin(radians);

            ctx.textAlign = "center";
            ctx.textBaseline = "middle";

            ctx.fillText(`${angle}°`, x, y
            );
        }
    );
}

//----------------------Range labels---------------------------

function drawRangeLabels(centerX, centerY, maxRadius)
{
    const ranges = 
    [
        maxRange / 4,
        maxRange / 2,
        maxRange * 3 / 4,
        maxRange
    ];

    ranges.forEach(function(distance)
    {
        const radius = (distance / maxRange) * maxRadius;
        ctx.textAlign = "center";

        ctx.fillText(
            `${distance} cm`,
            centerX,
            centerY - radius
        );
    });
}

//-----------------------------Detection points---------------------------

function drawDetections()
{
    detections.forEach(function(detection)
    {
        const point = polarToCanvas(detection.angle, detection.distance);

        ctx.beginPath();
        ctx.arc(point.x, point.y, 4, 0, 2 * Math.PI);

        ctx.fill();
    });
}

//-------------------------Sweep line-------------------------

function drawSweepLine(centerX, centerY, maxRadius)
{
    const radians = currentAngle * Math.PI / 180;

    const x = centerX + maxRadius * Math.cos(radians);

    const y = centerY - maxRadius * Math.sin(radians);

    ctx.beginPath();
    ctx.moveTo(centerX, centerY);

    ctx.lineTo(x, y);

    ctx.stroke();
}

//--------------------------Canvas resizing--------------------------------

function resizeCanvas()
{
    const rect = canvas.getBoundingClientRect();

    const width = Math.max(300, Math.floor(rect.width));

    const height = Math.max(200, Math.floor(rect.height));

    /*
    * Only resize when necessary.
    */

    if (canvas.width !== width || canvas.height !== height)
    {
        canvas.width = width;
        canvas.height = height;
    }
}

//-------------------Window resize---------------------------

window.addEventListener("resize", function(){drawRadar();});

//------------------------Initial state---------------------------

updateDisplays();

drawRadar();

connectWebSocket();