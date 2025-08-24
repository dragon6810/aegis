#include "ImplVulkan.h"

#include <utilslib.h>

void renderer::DescLayout::AddBinding(int bindnum, type_e type)
{
    VkDescriptorSetLayoutBinding bindinfo;

    bindinfo = {};
    bindinfo.binding = bindnum;
    bindinfo.descriptorCount = 1;
    bindinfo.descriptorType = (VkDescriptorType) type;

    impl->vkbindings.push_back(bindinfo);
}

void renderer::DescLayout::Build(shaderstage_e stage)
{
    int i;

    VkDescriptorSetLayoutCreateInfo layoutinfo;

    for(i=0; i<impl->vkbindings.size(); i++)
        impl->vkbindings[i].stageFlags |= stage;

    layoutinfo = {};
    layoutinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutinfo.pBindings = impl->vkbindings.data();
    
    UTILS_ASSERT(vkCreateDescriptorSetLayout(renderer->impl->device, &layoutinfo, NULL, &impl->vklayout) == VK_SUCCESS);
}

void renderer::DescLayout::Init(Renderer* renderer)
{
    UTILS_ASSERT(renderer);

    this->renderer = renderer;
    this->impl = std::make_unique<Impl>();
}

void renderer::DescLayout::Shutdown(void)
{

}