
## VulkanToGo
VulkanToGo is intended to be a convenient way to get started with Vulkan, by abstacting away a lot of the boilerplate code necessary to start rendering.
It comes with a handful of tools for various standard Vulkan tasks, such as pipeline and descriptor creation, buffer and image creation, data transfer and more.
All of these are opt-in and you can pick and choose what seems most useful for your work.


## Compilation
Just copy the vulkantogo folder into your project and add it to your CMake build tree.


## Dependencies
[Vulkan-Hpp](https://github.com/KhronosGroup/Vulkan-Hpp) for Vulkan C++ bindings. \
[GLFW](https://github.com/glfw/glfw) for window creation and input polling. \
[Vulkan Memory Allocator](https://github.com/YaaZ/VulkanMemoryAllocator-Hpp) for efficient memory allocation.


## Contents
+ [Core](#Core)
+ [Swapchain](#Swapchain)
+ [Pipelnes](#Pipelnes)
+ [Descriptors](#Descriptors)
+ [Rendering](#Rendering)
+ [Commands](#Commands)
+ [Submit Context](#Submit-Context)
+ [Storage](#Storage)
+ [Transfer](#Transfer)
+ [Synchronization](#Synchronization)
+ [Samplers](#Samplers)
+ [Utilities](#Utilities)


## Core
This is where all the initial setup happens, which includes GLFW window creation, Vulkan instance creation, GPU selection, device creation and Queue selection, as well as vma allocator creation.
All of these are exposed via functions returning static variables, which are created on first call. \
Furthermore there are three important functions you need to call once:

__vktg::Config()__ : Returns a pointer to the __vktg::ConfigSettings__ object. There you can specify initial window dimensions, whether to use fullscreen and provide your own functions to customize GLFW settings, required Vulkan extensions and device features you want to use. Some reasonable defaults are used for these settings, so this call is optional if you just want to do some quick rendering or don't use any advanced techniques. \
__vktg::StartUp()__ : The first thing you need to call after configuration. It makes sure all the fundamental Vulkan objects are initialized in the correct order. \
__vktg::ShutDown()__ : The last call you make after you are done rendering. It makes sure everything is cleaned up and destroyed in the proper order.

Furthermore use __vktg::Window()__ to access the GLFW window, __vktg::Allocator()__ to access the vma allocator and __vktg::Instance()__, __vktg::Surface()__, __vktg::Gpu()__, __vktg::Device()__ to access the respective Vulkan objects. Vulkan queues are accessed with the __vktg::GraphicsQueue()__, __vktg::ComputeQueue()__ and __vktg::TransferQueue()__ functions. Some or all of these may be the same queue depending on your system.


## Swapchain
Swapchains are handled with the __vktg::Swapchain__ class, holding a Vulkan swapchain, images, image views and relevant metadata for easy (re-)creation. A flag to mark the swapchain as invalid is included to indicate the need for swapchain recreation.

__vktg::PrepareSwapchain()__ : Returns a new uninitialized vktg::Swapchain. \
__vktg::CreateSwapchain(...)__ : Used to (re-)creates the swapchain, get the swapchain images and create the image views. \
__DestroySwapchain(...)__ : Destroys the swapchain and image views. 

There are also two convenience functions to handle swapchain image acquisition and presentation:

__vktg::NextSwapchainImage(...)__ : Used to retrieve the index of the next swapchain image. If this fails because the swapchain is out of date the vktg::Swapchain will be marked as invalid. \
__vktg::PresentImage(...)__ : Used to present the next swapchain image, If this failes because the swapchain is out of date, suboptimal or the window size has changed the vktg::Swapchain will be marked as invalid.


## Pipelines
Pipelines are handled with the __vktg::Pipeline__ class, storing the pipeline type, Vulkan pipeline and pipeline layout. The __vktg::\*PipelineBuilder__ classes provide a convenient way to customize pipeline stages, set shader modules and pipeline layout before calling the __Build()__ function to create the vktg::Pipeline object. \
VulkanToGo uses dynamic rendering as of Vulkan 1.3, so the graphics pipeline builder uses a __vk::PipelineRenderingCreateInfo__ and color attachment formats instead of a __vk::Renderpass__ .

If you don't want to use the pipeline builders you can still create pipelines and pipeline layouts the straightforward way: 

__vktg::Create\*Pipeline(...)__ : Creates a Vulkan pipeline from a pipeline create info for compute and graphics pipelines respectively. \
__vktg::CreatePipelineLayout(...)__ : Creates a Vulkan pipeline layout from a list of descriptor set layouts and a list of push constant ranges.

Shader modules can be loaded from a file path using the __vktg::LoadShader(...)__ function, or created from byte code with  __vktg::CreateShaderModule(...)__ if you use your own file system.


## Desriptors
Descriptor set allocation is managed by the __vktg::DescriptorSetAllocator__ class. This will automatically create and store descriptor pools of specified size and allocate descriptor sets from the current free pool.

__DescriptorSetAllocator(...)__ : Initializes the descriptor set allocator with given pool sizes and maximum sets per pool. If you don't care to specify pool sizes the default constructor will set the pool sizes to create descriptor pools with a large number of each descriptor type, which can be inefficient. \
__Allocate(...)__ : Allocates a new descriptor set from the current pool. \
__ResetPools()__ : Resets all created descriptor pools. \
__DestroyPools()__ : Destroys all created descriptor pools.

Descriptor set layouts are created and stored by the __vktg::DescriptorLayoutCache__ class.

__CreateLayout(...)__ : Create a descriptor set layout from a layout create info or a list of layout bindings, or return the layout with the same bindings already in the cache. \
__DestroyLayouts()__ : Destroys all the cashed descriptor set layouts.

Descriptor sets can be conveniently allocated and updated with the __vktg::DescriptorSetBuilder__ class. That is particularly useful for descriptors that change frequently and need to be updated each frame.

__DescriptorSetBuilder(...)__ : Iitializes the descriptor set builder using a given descriptor set allocator and layout cache. \
__BindBuffer(...)__ and __BindImage(...)__ : Adds a Vulkan buffer or image to the specific descriptor binding of the set you are building. \
__Build(...)__ : Allocates and updates the descriptor set. Also provides the option to store the layout of the created descriptor set by submitting a pointer.

If you want to create standalone descriptor pools, layouts and sets you can use :

__vktg::CreateDescriptorPool(...)__ : Creates a descriptor pool with a list of pool sizes, maximum  number of sets and creation flags. \
__vktg::CreateDescriptorSetLayout(...)__ : Creates a descriptor set layout from a list of layout bindings.
__vktg::AllocateDescriptorSet(...)__ : Allocates a descriptor set of specified layout from a given pool.

The descriptor buffer and image infos necessary for descriptor updates can be conveniently created with :
__vktg::GetDescriptorBufferInfo(...)__ : Creates a descriptor info for a range of a given buffer.
__vktg::GetDescriptorImageInfo(...)__ : Creates a descriptor info for a given image view, with an optional sampler for combined image sampler descriptors.


___NOTE:__ The DescriptorAllocator and DescriptorLayoutCache are not thread-safe, so it is recommended to create one for each thread that handles descriptor set allocation. Furthermore each overlapping frame should get its own descriptor allocator, to enable independend pool resets without breaking things._


## Rendering
Color and depth-stencil attachments for dynamic rendering are created with __vktg::CreateColorAttachment(...)__ and __vktg::CreateDepthStencilAttachment(...)__. Clear color and depth-stencil values are created with __vktg::CreateClearColorValue(...)__ and __vktg::CreateClearDepthStencilValue(...)__. Finally use __vktg::CreateRenderingInfo(...)__ to create the rendering info for dynamic rendering to an area of given extent, specifying a list of color attachments and optional depth-stencil attachment. \
_Separate depth and stencil attachments are not yet supported in Vulkan but the option is reserved if that changes in the future._

The need to perform image layout transitions is ubiquitous in Vulkan especially when using dynamic rendering instead of render passes. To simplify the layout transition use __vktg::TransitionImageLayout(...)__ tp place an image memory barrier for the given image in the command buffer, specifying old and new layout, pipeline stages and memory access for the source and destination stage.


## Commands
Use __vktg::CreateCommandPool(...)__ to create a command pool for a queue family of given index and __vktg::AllocateCommandBuffer(...)__ to allocate a command buffer from a given command pool. Submission of a list of command buffers to a specific queue is done via __vktg::SubmitCommands(...)__ with optional lists of wait and signal semaphores and a fence to signal once the command have been executed is done.


## Submit Contexts
The __vktg::SubmitContext__ class is a collection of a Vulkan queue, command pool, command buffer and fence and is intended to handle command submissions outside the main render loop, especially for compute and transfer outside  the graphics queue.

__vktg::CreateSubmitContext(...)__ : Creates a submit context for a given queue type. \
__vktg::DestroySubmitContext(...)__ : Destroys a given submit context i.e. the command pool and fence held by it. \

Command submission is started with a call to the __Begin()__ function, which resets the fence and command buffer before putting the command buffer into recording state again. Command recording is finished with a call to __End()__ which simply closes the command buffer. Finally the recorded commands can be submitted to the submit contexts queue using __Submit()__. 


## Storage
Buffers are handled with the __vktg::Buffer__ class. It holds a Vulkan buffer and all information used to create it and allcate buffer memory with vma. It provides functions to check buffer size, usage and a pointer to the buffer memory if it is a CPU-visible buffer and mapped memory is requested on creation.

__vktg::CreateBuffer(...)__ : Creates a Vulkan buffer of requested size, usage and memory usage and stores it in a vktg::Buffer object alongside its creation info and allocation info. Additional flags can be provided to optimize memory usage. An optional list of queue family indices can be submitted if the buffer is created with concurrent queue sharing mode, but by default buffers are exclusive to one queue. \
__vktg::ResizeBuffer(...)__ : Utility function to destroy and recreate a buffer with new size, but keeping all the other settings it had on first creation. \
__vktg::CreateStagingBuffer(...)__ : Utility function to create a CPU visible buffer used as a transfer source for data upload (staging buffer) to buffer in GPU memory. An optional pointer can be submitted to immediately memcopy data to the created staging buffer. \
__vktg::DestroyBuffer(...)__ : Destroys the buffer held by a vktg::Buffer and frees the buffer memory.

Images are handled with the __vktg::Image__ class. It holds a Vulkan image, image view over the whole range of image layers and mip levels and all information used to create them and allcate image memory with vma. It provides functions to check image dimensions, usage, number of mip levels and image layers and a pointer to the image memory if it is a CPU-visible image and mapped memory is requested on creation.

__vktg::CreateImage(...)__ : Creates a Vulkan image of requested width, height, format, layers, mip levels, sample count, usage and memory usage and stores it in a vktg::Image object. Also creates a Vulkan image view over the entire image with given image aspect. Additional flags can be provided to optimize memory usage. An optional list of queue family indices can be submitted if the image is created with concurrent queue sharing mode, but by default images are exclusive to one queue. \
__vktg::ResizeImage(...)__ : Utility function to destroy and recreate an image with new width and height, but keeping all the other settings it had on first creation. \
__vktg::DestroyImage(...)__ : Destroys the image and image view held be a vktg::Image and frees the image memory.

Additional image views over a given Vulkan image, covering a range of layers and mip levels can be created with __vktg::CreateImageView(...)__.


## Transfer
Provides functions to copy data between Vulkan buffers and images.

__vktg::CopyBuffer(...)__ : Records a copy command of a buffer region or list of buffer regions from a source buffer to a destination buffer in a given command buffer. \
__vktg::CopyImage(...)__ : Records a copy command of an image region from a source image to a destination image in a given command buffer, applying a given filter if the region sizes don't match. \
__vktg::CopyBufferToImage(...)__ : Records a copy command of a data from a source buffer to a destination image. The size of the buffer region copied is equal the the width times height of the destination image region. \
__vktg::CopyImageToBuffer(...)__ : Records a copy command of a data from a source image to a destination buffer. The size of the buffer region copied to equal the the width times height of the source image region. \

Additional functions for direct upload of buffer and image data to the GPU are provided. These are blocking functions that will wait for completion of the submitted copy command and should not be used in your main render loop thread, but rather in their own thread. 

__vktg::UploadBufferData(...)__ : Uploads data from a given memory location to a buffer region of given size and offset.\
__vktg::UploadImageData(...)__ : Uploads data from a given memory location to an image region of given width, height and offset. 


## Synchronization
Vulkan fences are created using __vktg::CreateFence(...)__ and destroyed with __vktg::DestroyFence(...)__. You can wait for one or multiple fences using __WaitForFence(...)__ and __WaitForFences(...)__ respectively. Fence resets are performed using __vktg::ResetFence(...)__ and __vktg::ResetFences(...)__.

Vulkan semaphores are created with __vktg::CreateSemaphore(...)__ specifying wether you want a binary or timeline semaphore and destroyed with __vktg::DestroySemaphore(...)__. Timeline semaphores can be signaled using __vktg::SignalSemaphore(...)__.

Furthermore Vulkan memory barriers, buffer memory barriers and image memory barriers are created using __vktg::CreateMemoryBarrier(...)__, __vktg::CreateBufferMemoryBarrier(...)__ and __vktg::CreateImageMemoryBarrier(...)__.


## Samplers
The __vktg::SamplerBuilder__ class is used as a convenient way to customize and create Vulkan samplers. It provides functions to configure all the desired sampler setting before calling __Build()__ to create and return the Vulkan sampler.

Alternatively you can create a sampler from a sampler create info using __vktg::CreateSampler(...)__ and destroyed with __vktg::DestroySampler(...)__.


## Utilities
VulkanToGo also comes with some utitilies that could be handy for quick prototyping. You will likely want to roll out your own version of some of these more tailored to your specific use case.

#### Deletion Stack
The __vktg::DeletionStack__ class is used to ensure that the destruction of Vulkan objects and memory deallocation happens in the correct order. Whenever you create any Vulkan object you can push the respective deletion function onto the stack and flush the stack once you are done, which calls all submitted deletion functions in reverse order. \
In practice you want to use one deletion stack for everyting that is created once and doesn't need recreation for the whole runtime and one deletion stack for each overlapping frame for everything that has a single frame lifetime.
Destruction of object that need infrequent recreation like swapchains, render images or scene data storage buffers should be handled by the deletion stack.

#### Timer
The __vktg::Timer__ class is a simple way to measure time, which also enables time scaling for slow-down or fast-forward effects. You can access the time delta, the total elapsed time (scaled and unscaled) and even the current date-time stamp, which can be useful for logging.

#### Input handler
The __vktg::InputHandler__ class polls GLFW events and stores the key and mouse button states as well as the current cursor position and position delta. These can be checked directly using the __KeyPressed(...)__, __MouseButtonPressed(...)__, __CursorPos()__ and __CursorDelta()__ functions. \
Alternatively you can use the __vktg::InputLayer__ class to set custom functions to handle key, mouse and cursor input. Input layers can be submitted to and removed from the input handler with the __Push(...)__ and __Pop()__ functions and only the top layer processes the input.

#### Frame Handler
The __vktg::FrameHandler__ class keeps track of the frame overlap and total frame count and lets you get the current frame index. It also comes with a timer to get the frame time delta. In addition you can register callbacks to call at the beginning/end of each frame, for example to display FPS.