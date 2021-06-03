#include "RenderStageBase.hpp"
#include "Graphics.hpp"

namespace pvk::engine
{
RenderStageBase::RenderStageBase()
{
}

RenderStageBase::~RenderStageBase()
{
    m_descriptorPool.reset();
    m_pipeline.reset();
}
} // namespace pvk::engine