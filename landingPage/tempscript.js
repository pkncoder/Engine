/**
 * Tempname: Engine - Core Site Data Strategy Workspace
 */

// 1. Developmental Flowchart Input System
const developmentalPipelineData = [
    {
        phase: "Phase I",
        status: "done",
        badgeText: "Done",
        items: ["Project environment config", "First window & core program initialization"]
    },
    {
        phase: "Phase II",
        status: "done",
        badgeText: "Done",
        items: ["Hello Triangle compilation target", "Shader pipeline setup"]
    },
    {
        phase: "Phase III",
        status: "done",
        badgeText: "Exited",
        items: ["Basic Scene handling framework", "Asset Manager instantiation", "Base Rasterizer & Path Tracer engine tests"]
    },
    {
        phase: "Phase IV",
        status: "active",
        badgeText: "Active Target",
        items: ["Comprehensive Scene Management modules", "Professional Rendering Architecture", "Advanced Raster/Ray/Path/March Shaders", "Spatial Acceleration Trees", "File Import & Export Setup"]
    },
    {
        phase: "Phase V",
        status: "planned",
        badgeText: "Planned",
        items: ["UI/UX Configuration Layer", "Full User Experience"]
    },
    {
        phase: "Phase VI",
        status: "planned",
        badgeText: "Planned",
        items: ["Architectural Sweep (\"Rewire\")", "Professional Release"]
    }
];

// 2. Data Driven Laboratory Demos Map
const onlineDemosData = [
    {
        name: "Ray Tracer",
        devState: "not-ported",
        badgeText: "Not Ported",
        context: "Light estimation against primitive intersections, basic diffuse, specular reflections (mirrors), refractive reflections (transparency), diffuse materials, and explicit shadow vectors.",
        context: "Resolves discrete spatial primitive intersections, basic specular reflections, and explicit shadow vectors.",
        link: "https://www.shadertoy.com/view/tXyXRc"
    },
    {
        name: "Path Tracer",
        devState: "active",
        badgeText: "Working On It",
        context: "Global illumination solver calculating diffuse interreflections and soft shadow values through progressive stochastic Monte Carlo logic routines, MIS, and NEE.",
        link: "https://www.shadertoy.com/view/7fBSzR"
    },
    {
        name: "Ray Marcher",
        devState: "not-made",
        badgeText: "Not Made",
        context: "Mathematical processing of Signed Distance Fields (SDFs) optimized to evaluate volumetric phenomena like procedural clouds, and infinate spaces like fractals.",
        link: "#"
    },
    {
        name: "Ray Casting (Voxels)",
        devState: "not-made",
        badgeText: "Not Made",
        context: "Discrete block environment traversal pipelines optimizing fast grid space tracking indexes without polygon mesh load weight overhead.",
        link: "#"
    },
    {
        name: "Marching Cubes",
        devState: "not-made",
        badgeText: "Not Made",
        context: "Volumetric geometry & mesh generation, converting discrete voxel field data maps into unified vertex triangle array layouts.",
        link: "#"
    }
];

// 3. Render Capture Library Array Matrix Mapping
const renderCaptureLibraryData = {
    "Sponza 1": "./assets/Sponza-1.png",
    "Breakfast Room 1": "./assets/BreakfastRoom-1.png",
    "Backrooms 1": "./assets/Backrooms-1.png",
    "Lost Empire 1": "./assets/LostEmpire-1.png",
    "Sponza 2": "./assets/Sponza-2.png",
    "Queer Room 1 (PT)": "./assets/QueerRoom-1.png",
    "Backrooms 2": "./assets/Backrooms-2.png",
    "Lost Empire 2": "./assets/LostEmpire-2.png",
    "Sponza 3": "./assets/Sponza-3.png",
};

/**
 * DOM Core Generation Lifecycles
 */
document.addEventListener("DOMContentLoaded", () => {
    generateDevelopmentPipeline();
    generateOnlineDemos();
    generateRenderCaptureLibrary();
});

/**
 * Pipeline Generation Controller
 */
function generateDevelopmentPipeline() {
    const container = document.getElementById("pipelineScroller");
    if (!container) return;

    if (!developmentalPipelineData || developmentalPipelineData.length === 0) {
        container.innerHTML = `
            <div class="w-100 text-center py-4 text-secondary-custom fw-semibold">
                <i class="bi bi-exclamation-triangle me-2"></i>No developmental tracking data currently mapped.
            </div>`;
        return;
    }

    container.innerHTML = "";

    developmentalPipelineData.forEach((item, index) => {
        const cardNode = document.createElement("div");
        cardNode.className = `flowchart-card p-3 d-flex flex-column justify-content-between ${item.status}`;
        
        if (item.status === "active") {
            cardNode.id = "activePhaseCard";
        }

        let badgeStyle = "bg-secondary";
        if (item.status === "done") badgeStyle = "bg-success bg-opacity-20 text-dark";
        if (item.status === "active") badgeStyle = "text-dark style-active-badge";

        const listItemsHTML = item.items.map(subItem => `<li>${subItem}</li>`).join("");

        cardNode.innerHTML = `
            <div>
                <div class="d-flex justify-content-between align-items-center mb-2">
                    <h6 class="fw-bold mb-0 text-white">${item.phase}</h6>
                    <span class="badge small ${badgeStyle}">${item.badgeText}</span>
                </div>
                <ul class="text-secondary-custom small ps-3 mb-0">
                    ${listItemsHTML}
                </ul>
            </div>
        `;

        container.appendChild(cardNode);

        if (index < developmentalPipelineData.length - 1) {
            const arrowNode = document.createElement("div");
            arrowNode.className = "flowchart-arrow";
            arrowNode.innerHTML = `<i class="bi bi-arrow-right"></i>`;
            container.appendChild(arrowNode);
        }
    });

    setTimeout(centerActiveTimelinePhase, 150);
}

function centerActiveTimelinePhase() {
    const scrollerContainer = document.getElementById("pipelineScroller");
    const targetActiveCard = document.getElementById("activePhaseCard");
    
    if (scrollerContainer && targetActiveCard) {
        const containerHalfWidth = scrollerContainer.clientWidth / 2;
        const cardHalfWidth = targetActiveCard.clientWidth / 2;
        const targetScrollLeftPosition = targetActiveCard.offsetLeft - containerHalfWidth + cardHalfWidth;
        scrollerContainer.scrollLeft = targetScrollLeftPosition;
    }
}

/**
 * Online Demo Sandbox Workspace Module Builder
 */
function generateOnlineDemos() {
    const container = document.getElementById("demosContainer");
    if (!container) return;

    if (!onlineDemosData || onlineDemosData.length === 0) {
        container.innerHTML = `
            <div class="w-100 text-center py-4 text-secondary-custom fw-semibold">
                <i class="bi bi-exclamation-triangle me-2"></i>No sandbox rendering components currently registered.
            </div>`;
        return;
    }

    container.innerHTML = "";

    onlineDemosData.forEach(demo => {
        const columnWrapper = document.createElement("div");
        columnWrapper.className = "col-md-6 col-lg-4";

        let badgeClass = "bg-secondary text-white";
        let cardModifierClass = "";
        let actionFooterElement = "";

        if (demo.devState === "active") {
            badgeClass = "text-dark style-active-badge";
            actionFooterElement = `<a href="${demo.link}" target="_blank" class="btn btn-sm btn-outline-danger w-100">Launch Prototype Instance</a>`;
        } else if (demo.devState === "not-ported") {
            badgeClass = "text-white style-notported-badge";
            actionFooterElement = `<a href="${demo.link}" target="_blank" class="btn btn-sm btn-outline-danger w-100">Launch Prototype Instance</a>`;
        } else if (demo.devState === "not-made") {
            badgeClass = "bg-secondary text-white";
            cardModifierClass = "style-planned opacity-75";
            actionFooterElement = `<button class="btn btn-sm btn-dark w-100" disabled>Concept Phase</button>`;
        }

        columnWrapper.innerHTML = `
            <div class="card h-100 d-flex flex-column justify-content-between ${cardModifierClass}">
                <div class="card-body">
                    <div class="d-flex justify-content-between align-items-start mb-2 gap-2">
                        <h5 class="card-title text-white fw-bold mb-0">${demo.name}</h5>
                        <span class="badge small ${badgeClass}" style="white-space: nowrap;">${demo.badgeText}</span>
                    </div>
                    <p class="card-text text-secondary-custom small mb-0">
                        ${demo.context}
                    </p>
                </div>
                <div class="card-footer bg-transparent border-0 pb-3">
                    ${actionFooterElement}
                </div>
            </div>
        `;
        container.appendChild(columnWrapper);
    });
}

/**
 * Framerate Independent Adaptive Image Gallery Loop Engine
 */
function generateRenderCaptureLibrary() {
    const viewport = document.getElementById("galleryViewport");
    const track = document.getElementById("galleryTrack");
    if (!viewport || !track) return;

    const imgKeys = Object.keys(renderCaptureLibraryData);

    if (imgKeys.length === 0) {
        track.innerHTML = `
            <div class="p-4 text-center text-secondary-custom w-100">
                <i class="bi bi-image-alt me-2"></i>Render capture array stack is currently empty.
            </div>`;
        return;
    }

    track.innerHTML = "";

    const appendItems = (targetContainer) => {
        imgKeys.forEach(altName => {
            const imgPath = renderCaptureLibraryData[altName];
            const itemNode = document.createElement("div");
            itemNode.className = "card gallery-item rounded overflow-hidden shadow-sm";
            
            itemNode.innerHTML = `
                <div class="gallery-img-container">
                    <img src="${imgPath}" alt="${altName}" class="w-100 d-block" style="height:160px; object-fit: cover;" 
                         onerror="this.onerror=null; this.parentElement.innerHTML='<div class=\\'gallery-img-placeholder\\'><i class=\\'bi bi-file-earmark-code fs-3\\'></i></div>';">
                </div>
                <div class="p-2 bg-dark border-top border-secondary text-center">
                    <span class="small text-secondary-custom fw-semibold">${altName}</span>
                </div>
            `;

            // Setup Lightbox Trigger Listeners
            itemNode.addEventListener("click", () => {
                const lightboxImg = document.getElementById("lightboxImage");
                const lightboxTitle = document.getElementById("lightboxTitle");
                if (lightboxImg && lightboxTitle) {
                    lightboxImg.src = imgPath;
                    lightboxTitle.textContent = altName;
                    
                    const bootstrapModalInstance = new bootstrap.Modal(document.getElementById('lightboxModal'));
                    bootstrapModalInstance.show();
                }
            });

            targetContainer.appendChild(itemNode);
        });
    };

    // Build base track list + duplicate layout tracking layers to support clean infinite looping
    appendItems(track);
    appendItems(track);

    initializeDeltaTimeMarquee(viewport, track);
}

/**
 * Frame-Independent Linear Interpolation Velocity Controller
 */
function initializeDeltaTimeMarquee(viewport, track) {
    let currentScrollOffset = 0;
    let lastTimestamp = 0;
    
    // Smooth Interpolation State Tracking Weights
    let speedWeightScalar = 1.0; 
    let targetWeightScalar = 1.0;
    
    const baseVelocityPxPerSec = 45; // Fixed spatial movement scaling factor constant

    // Event Hover State Anchors
    viewport.addEventListener("mouseenter", () => targetWeightScalar = 0.0);
    viewport.addEventListener("mouseleave", () => targetWeightScalar = 1.0);

    function executionRenderLoop(currentTimestamp) {
        if (!lastTimestamp) lastTimestamp = currentTimestamp;
        
        // Calculate the standard delta time conversion metric factor
        const deltaTimeSeconds = (currentTimestamp - lastTimestamp) / 1000;
        lastTimestamp = currentTimestamp;

        // Linear interpolation framework modeling ease-in-out movement behaviors
        const easeBlendRate = 6.0;
        speedWeightScalar += (targetWeightScalar - speedWeightScalar) * easeBlendRate * deltaTimeSeconds;

        // Compute runtime frame shifts safely
        currentScrollOffset += baseVelocityPxPerSec * deltaTimeSeconds * speedWeightScalar;
        
        // Midpoint wrap calculation boundaries
        if (currentScrollOffset >= (track.scrollWidth / 2)) {
            currentScrollOffset = 0;
        }
        
        viewport.scrollLeft = currentScrollOffset;
        requestAnimationFrame(executionRenderLoop);
    }

    requestAnimationFrame(executionRenderLoop);
}
