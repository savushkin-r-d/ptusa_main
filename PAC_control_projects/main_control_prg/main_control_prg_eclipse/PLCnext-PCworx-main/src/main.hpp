#pragma once
#include "Arp/System/Core/Arp.h"
#include "Arp/System/Acf/ComponentBase.hpp"
#include "Arp/System/Acf/IApplication.hpp"
#include "Arp/Plc/Esm/IProgramComponent.hpp"

#include "mainProgramProvider.hpp"

using namespace Arp;
using namespace Arp::System::Acf;
using namespace Arp::Plc::Esm;

namespace ptusa
{

class main : public ComponentBase, public IProgramComponent
{
public: // typedefs

public: // construction/destruction
    main(IApplication& application, const String& name);
    virtual ~main(void) = default;

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
    main(const main& arg) = delete;
    main& operator= (const main& arg) = delete;

private: // fields
    mainProgramProvider programProvider;
};

///////////////////////////////////////////////////////////////////////////////
// inline methods of class SimulinkMode1Component
inline main::main(IApplication& application, const String& name)
: ComponentBase(application, name, ComponentCategory::Custom, Version(0))
{
}

inline IComponent::Ptr main::Create(IApplication& application, const String& componentName)
{
    return IComponent::Ptr(new main(application, componentName));
}

inline IProgramProvider* main::GetProgramProvider()
{
    return &programProvider;
}

} // end of namespace ptusa
