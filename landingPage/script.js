/**
 * Tempname: Engine - Core Client Pipeline Controller Map
 * Driven completely through jQuery DOM API compilation contexts.
 */

// 1. Developmental Flowchart Input System Data Model
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
        context: "Mathematical processing of Signed Distance Fields (SDFs) optimized to evaluate volumetric phenomena like procedural clouds, and infinite spaces like fractals.",
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

// Execute UI Compilation on Document Ready Context Initialization Loop
$(document).ready(function() {

    /* ==========================================================================
       Compilation Function: Flowchart Tree Structuring Map
       ========================================================================== */
    function compilePipelineTree() {
        const $scroller = $('#pipelineScroller');
        $('#pipelinePlaceholder').remove(); // Evict active loader thread text

        const $treeContainer = $('<div>', { class: 'flowchart-tree-track d-flex gap-4 pb-2' });

        $.each(developmentalPipelineData, function(index, phaseNode) {
            const $nodeCard = $('<div>', { class: `flowchart-card card p-3 flex-shrink-0 ${phaseNode.status}` });
            
            // Setup dynamic phase status text header frames
            let statusColorRule = 'bg-secondary';
            let activatedPhaseCard = false;
            if (phaseNode.status === 'done') statusColorRule = 'state-done-badge';
            if (phaseNode.status === 'active') { statusColorRule = 'state-active-badge'; activatedPhaseCard = true; }

            let nodeHeaderHtml = `
                <div class="d-flex justify-content-between align-items-center mb-2" ${activatedPhaseCard ? "id=\"activePhaseCard\"" : ""}>
                    <h5 class="text-white fw-bold mb-0">${phaseNode.phase}</h5>
                    <span class="badge ${statusColorRule} small text-dark">${phaseNode.badgeText}</span>
                </div>
                <ul class="mb-0 text-secondary-custom ps-3 small">
            `;

            $.each(phaseNode.items, function(i, itemText) {
                nodeHeaderHtml += `<li class="mb-1">${itemText}</li>`;
            });
            nodeHeaderHtml += `</ul>`;

            $nodeCard.append(nodeHeaderHtml);
            $treeContainer.append($nodeCard);

            if (index < developmentalPipelineData.length - 1) {
                const arrowNode = $("<div>", { class: `flowchart-arrow`} );
                arrowNode.append($("<i>", { class: `bi bi-arrow-right` }));
                $treeContainer.append(arrowNode);
            }
        });

        $scroller.append($treeContainer);

        centerActiveTimelinePhase();
        evaluateFlowchartScrollShadows(); // Refresh context bounding tracking boxes
    }

    /* ==========================================================================
       Context Monitoring: Flowchart Scroll Shadow Controller Loops
       ========================================================================== */
    const $scrollerElement = $('#pipelineScroller');
    const $wrapperElement = $('#flowchartWrapper');

    function centerActiveTimelinePhase() {
        const targetActiveCard = $("#activePhaseCard");

        if ($scrollerElement[0] && targetActiveCard[0]) {
            const containerHalfWidth = $scrollerElement[0].clientWidth / 2;
            const cardHalfWidth = targetActiveCard[0].clientWidth / 2;
            const targetScrollLeftPosition = -(targetActiveCard[0].offsetLeft - containerHalfWidth + cardHalfWidth);
            $scrollerElement.scrollLeft(targetScrollLeftPosition);
        }
    }

    function evaluateFlowchartScrollShadows() {
        const scrollLeftValue = $scrollerElement.scrollLeft();
        const maxScrollWidth = $scrollerElement[0].scrollWidth - $scrollerElement.outerWidth();

        // Check bounds context allocations to trigger overlay classes
        if (scrollLeftValue > 5) {
            $wrapperElement.addClass('left-shadow-active');
        } else {
            $wrapperElement.removeClass('left-shadow-active');
        }

        if (scrollLeftValue < maxScrollWidth - 5) {
            $wrapperElement.addClass('right-shadow-active');
        } else {
            $wrapperElement.removeClass('right-shadow-active');
        }
    }

    // Attach high-rate polling listener event targets using jQuery delegate structures
    $scrollerElement.on('scroll', evaluateFlowchartScrollShadows);
    $(window).on('resize', evaluateFlowchartScrollShadows);

    /* ==========================================================================
       Compilation Function: Laboratory Code Verification Targets Grid
       ========================================================================== */
    function compileDemosGrid() {
        const $container = $('#demosContainer');
        $('#demosPlaceholder').remove();

        $.each(onlineDemosData, function(index, demoNode) {
            let stateBadgeColor = 'bg-secondary';
            if (demoNode.devState === 'active') stateBadgeColor = 'state-active-badge text-dark';
            if (demoNode.devState === 'not-ported') stateBadgeColor = 'bg-danger text-white';

            let actionButtonElement = `<a href="${demoNode.link}" target="_blank" class="btn btn-outline-light btn-sm w-100 mt-3 d-flex align-items-center justify-content-center gap-2"><i class="bi bi-box-arrow-up-right"></i>Open Online Demo</a>`;
            if (demoNode.link === '#') {
                actionButtonElement = `<button class="btn btn-dark btn-sm w-100 mt-3" disabled><i class="bi bi-lock-fill me-1"></i>Currently in Concept Phase</button>`;
            }

            const cardFrameMarkup = `
                <div class="col-md-6 col-xl-4">
                    <div class="card p-4 h-100 d-flex flex-column justify-content-between">
                        <div>
                            <div class="d-flex justify-content-between align-items-start mb-2 gap-2">
                                <h5 class="text-white fw-bold mb-0">${demoNode.name}</h5>
                                <span class="badge ${stateBadgeColor} small px-2 py-1">${demoNode.badgeText}</span>
                            </div>
                            <p class="text-secondary-custom small mb-0 lh-sm">${demoNode.context}</p>
                        </div>
                        ${actionButtonElement}
                    </div>
                </div>
            `;
            $container.append(cardFrameMarkup);
        });
    }

    /* ==========================================================================
       Compilation Function: Interactive Render Capture Laboratory Gallery Track
       ========================================================================== */
    function compileRenderGalleryTrack() {
        const $viewport = $('#galleryViewport');
        const $track = $('#galleryTrack');
        $('#galleryPlaceholder').remove();

        $.each(renderCaptureLibraryData, function(renderLabel, targetAssetPath) {
            const structuralFrameMarkup = `
                <div class="card gallery-item flex-shrink-0 position-relative rounded overflow-hidden cursor-pointer shadow-sm" data-title="${renderLabel}" data-asset="${targetAssetPath}">
                    <div class="gallery-img-container">
                        <img src="${targetAssetPath}" alt="${renderLabel}" class="w-100 h-100 d-block object-fit-cover" 
                            onerror="this.onerror=null; this.parentElement.innerHTML='<div class=\\'gallery-img-placeholder\\'><i class=\\'bi bi-file-earmark-code fs-3\\'></i></div>';">
                    </div>
                    <div class="gallery-overlay bottom-0 start-0 w-100 p-2 text-center fw-semibold bg-dark border-top border-secondary">
                        <span class="small text-secondary-custom fw-semibold">${renderLabel}</span>
                    </div>
                </div>
            `;
            $track.append(structuralFrameMarkup);
        });

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
            viewport.on("mouseenter", () => targetWeightScalar = 0.0);
            viewport.on("mouseleave", () => targetWeightScalar = 1.0);

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
                if (currentScrollOffset >= (track[0].scrollWidth / 2)) {
                    currentScrollOffset = 0;
                }

                viewport.scrollLeft(currentScrollOffset);
                requestAnimationFrame(executionRenderLoop);
            }

            requestAnimationFrame(executionRenderLoop);
        }

        initializeDeltaTimeMarquee($viewport, $track);

    }

    /* ==========================================================================
       Modal Operations Event Router Context Manager (Unified Lightbox Interceptor)
       ========================================================================== */
    const bootstrapLightboxInstance = new bootstrap.Modal($('#lightboxModal')[0]);

    // Track Interceptor 1: Click routing inside the compiled scrolling asset gallery pipeline
    $('#galleryTrack').on('click', '.gallery-item', function() {
        const selectedAssetUrl = $(this).attr('data-asset');
        const formattedTitleText = $(this).attr('data-title');

        $('#lightboxImage').attr('src', selectedAssetUrl);
        $('#lightboxTitle').text(`${formattedTitleText} — High Fidelity Native Capture Output`);
        bootstrapLightboxInstance.show();
    });

    // Track Interceptor 2: Direct hook connection to the new blueprint image architecture maps
    $(document).on('click', '.architecture-lightbox-trigger', function() {
        const directAssetUrl = $(this).attr('src');
        const backupDescriptionText = $(this).attr('alt');

        $('#lightboxImage').attr('src', directAssetUrl);
        $('#lightboxTitle').text(`${backupDescriptionText} — System Execution Blueprint Context`);
        bootstrapLightboxInstance.show();
    });

    /* ==========================================================================
       Pipeline Thread Initializer Invocation Triggers
       ========================================================================== */
    compilePipelineTree();
    compileDemosGrid();
    compileRenderGalleryTrack();


    const $viewport = $('#galleryViewport');
    const $track = $('#galleryTrack');

});
