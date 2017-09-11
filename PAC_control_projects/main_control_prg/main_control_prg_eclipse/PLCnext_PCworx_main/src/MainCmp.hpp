#pragma once
#include "Arp/System/Core/Arp.h"
#include "Arp/System/Acf/ComponentBase.hpp"
#include "Arp/System/Acf/IApplication.hpp"
#include "Arp/Plc/Esm/IProgramComponent.hpp"

#include "MainCmpProgramProvider.hpp"

using namespace Arp;
using namespace Arp::System::Acf;
using namespace Arp::Plc::Esm;

namespace PtusaLib
{

class MainCmp : public ComponentBase, public IProgramComponent
{
public: // typedefs

public: // construction/destruction
    MainCmp(IApplication& application, const String& name);
    virtual ~MainCmp(void) = default;

public: // static factory operations
    static IComponent::Ptr Create(IApplication& application, const String& componentName);

public: // IComponent operations
    void Initialize(void)override;
    void LoadSettings(const String& settingsPath)override;
    void SetupSettings(void)override;
    void SubscribeServices(void)override;
    void LoadConfig(void)override;
    void SetupConfig(void)override;
    void PublishServices(void)override;
    void Dispose(void)override;
    void PowerDown(void)override;

public: // IProgramProviderComponent operations
    IProgramProvider* GetProgramProvider(void)override;

private: // methods
    MainCmp(const MainCmp& arg) = delete;
    MainCmp& operator= (const MainCmp& arg) = delete;

private: // fields
    MainCmpProgramProvider programProvider;
};

///////////////////////////////////////////////////////////////////////////////
// inline methods of class SimulinkMode1Component
inline MainCmp::MainCmp(IApplication& application, const String& name)
: ComponentBase(application, name, ComponentCategory::Custom, Version(0))
{
}

inline IComponent::Ptr MainCmp::Create(IApplication& application, const String& componentName)
{
    return IComponent::Ptr(new MainCmp(application, componentName));
}

inline IProgramProvider* MainCmp::GetProgramProvider()
{
    return &programProvider;
}

} // end of namespace PtusaLib
