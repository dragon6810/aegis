#include "ImplVulkan.h"

#include <stdio.h>

#include <utilslib.h>

void renderer::Queue::SubmitCmdBuf(CmdBuf* buf, Semaphore* waitsem, Semaphore* signalsem, Fence* signalfence, stageflags_e waitstage, stageflags_e signalstage)
{
    VkSubmitInfo2KHR submitinfo;
    VkSemaphoreSubmitInfoKHR waitseminfo, signalseminfo;
    VkCommandBufferSubmitInfoKHR cmdbufinfo;

    UTILS_ASSERT(renderer);
    UTILS_ASSERT(impl.get());

    UTILS_ASSERT(buf);

    submitinfo = {};
    submitinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2_KHR;

    if(waitsem)
    {
        waitseminfo = {};
        waitseminfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR;
        waitseminfo.semaphore = waitsem->impl->vksem;
        waitseminfo.stageMask = waitstage;
        waitseminfo.deviceIndex = 0;
        waitseminfo.value = 1;

        submitinfo.waitSemaphoreInfoCount = 1;
        submitinfo.pWaitSemaphoreInfos = &waitseminfo;
    }

    if(signalsem)
    {
        signalseminfo = {};
        signalseminfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR;
        signalseminfo.semaphore = signalsem->impl->vksem;
        signalseminfo.stageMask = signalstage;
        signalseminfo.deviceIndex = 0;
        signalseminfo.value = 1;

        submitinfo.signalSemaphoreInfoCount = 1;
        submitinfo.pSignalSemaphoreInfos = &signalseminfo;
    }

    cmdbufinfo = {};
    cmdbufinfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO_KHR;
    cmdbufinfo.commandBuffer = buf->impl->cmdbuf;
	cmdbufinfo.deviceMask = 0;

    submitinfo.commandBufferInfoCount = 1;
    submitinfo.pCommandBufferInfos = &cmdbufinfo;

    if(signalfence)
        renderer->impl->queuesubmit2proc(impl->vkque, 1, &submitinfo, signalfence->impl->vkfence);
    else
        renderer->impl->queuesubmit2proc(impl->vkque, 1, &submitinfo, NULL);
}

void renderer::Queue::Init(type_e type, Renderer* renderer)
{
    this->renderer = renderer;
    this->impl = std::make_unique<Impl>();

    vkb::Result<VkQueue> queres = vkb::Result<VkQueue>(vkb::Error());
    vkb::Result<uint32_t> famres = vkb::Result<uint32_t>(vkb::Error());
    switch(type)
    {
    case TYPE_GFX:
        queres = renderer->impl->vkbdevice.get_queue(vkb::QueueType::graphics).value();
        famres = renderer->impl->vkbdevice.get_queue_index(vkb::QueueType::graphics).value();
        break;
    default:
        UTILS_ASSERT(0);
        break;
    }

    if(!queres)
    {
        printf("error %d getting vkQueue!\n", queres.vk_result());
        exit(1);
    }

    if(!queres)
    {
        printf("error %d getting vkQueueFamilyIndex!\n", famres.vk_result());
        exit(1);
    }

	impl->vkque = queres.value();
    impl->quefam = famres.value();
}

void renderer::Queue::Shutdown(void)
{
    
}
