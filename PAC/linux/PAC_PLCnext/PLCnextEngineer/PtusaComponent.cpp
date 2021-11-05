#include "PtusaComponent.hpp"
#include "Arp/Plc/Commons/Esm/ProgramComponentBase.hpp"
#include "PtusaLibrary.hpp"

namespace Ptusa
{

PtusaComponent::PtusaComponent(IApplication& application, const String& name)
: ComponentBase(application, ::Ptusa::PtusaLibrary::GetInstance(), name, ComponentCategory::Custom)
, programProvider(*this)
, ProgramComponentBase(::Ptusa::PtusaLibrary::GetInstance().GetNamespace(), programProvider)
{
}

void PtusaComponent::Initialize()
{
    // never remove next line
    ProgramComponentBase::Initialize();

    // subscribe events from the event system (Nm) here
}

void PtusaComponent::LoadConfig()
{
    // load project config here
}

void PtusaComponent::SetupConfig()
{
    // never remove next line
    ProgramComponentBase::SetupConfig();

    // setup project config here
}

void PtusaComponent::ResetConfig()
{
    // never remove next line
    ProgramComponentBase::ResetConfig();

    // implement this inverse to SetupConfig() and LoadConfig()
}

} // end of namespace Ptusa
