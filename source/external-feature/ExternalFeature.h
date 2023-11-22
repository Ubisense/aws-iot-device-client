// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef DEVICE_CLIENT_EXTERNALFEATURE_H
#define DEVICE_CLIENT_EXTERNALFEATURE_H

#include <aws/iot/MqttClient.h>

#include "../logging/LoggerFactory.h"
#include "../ClientBaseNotifier.h"
#include "../Feature.h"
#include "../SharedCrtResourceManager.h"
#include "../config/Config.h"
#include "../util/FileUtils.h"

#include "ExternalFeatureImpl.h"

using namespace Aws::Iot::DeviceClient::Logging;


namespace Aws
{
    namespace Iot
    {
        namespace DeviceClient
        {
            namespace External
            {
                /**
                 * \brief Provides IoT PubSub related sample functionality within the Device Client.
                 * When enabled The Pub Sub sample feature will publish data from a configured `publish-file` to a
                 * `publish-topic`, and will recieve messages on a configured `subscribe-topic` and write them to a
                 * `subscribe-file`. More information can be found in the `samples/pubsub/README.md`
                 */
                class ExternalFeature : public Feature
                {
                  public:
                    /**
                     * \brief Initializes the External feature with all the required setup information, event
                     * handlers, and the SharedCrtResourceManager
                     *
                     * @param manager The resource manager used to manage CRT resources
                     * @param notifier an ClientBaseNotifier used for notifying the client base of events or errors
                     * @param config configuration information passed in by the user via either the command line or
                     * configuration file
                     * @return a non-zero return code indicates a problem. The logs can be checked for more info
                     */
                    int init(
                        std::shared_ptr<SharedCrtResourceManager> manager,
                        std::shared_ptr<ClientBaseNotifier> notifier,
                        const PlainConfig &config)
                    {
                        string thingName = config.thingName->c_str();
                        resourceManager = manager;
                        baseNotifier = notifier;
                        impl = std::make_shared<ExternalFeatureImpl>();
                        impl->init(thingName, manager->getConnection());
                        return AWS_OP_SUCCESS;
                    }

                    // Interface methods defined in Feature.h
                    std::string getName() override
                    {
                        return impl->getName();
                    }

                    int start() override
                    {
                        // \todo failure?
                        int result = impl->start();
                        LOGM_INFO("external-feature/ExternalFeature.h", "Starting %s, result=%d", getName().c_str(), result);

                        baseNotifier->onEvent(static_cast<Feature *>(this), ClientBaseEventNotification::FEATURE_STARTED);
                        return AWS_OP_SUCCESS;
                    }
                    int stop() override
                    {
                        // \todo failure?
                        int result = impl->stop();
                        LOGM_INFO("external-feature/ExternalFeature.h", "Stopping %s, result=%d", getName().c_str(), result);
                        baseNotifier->onEvent(static_cast<Feature *>(this), ClientBaseEventNotification::FEATURE_STOPPED);
                        return AWS_OP_SUCCESS;
                    }

                  private:
                    /**
                     * \brief Pointer to external feature implementation instance.
                    */
                    std::shared_ptr<ExternalFeatureImpl> impl;

                    /**
                     * \brief The resource manager used to manage CRT resources
                     */
                    std::shared_ptr<SharedCrtResourceManager> resourceManager;
                    /**
                     * \brief An interface used to notify the Client base if there is an event that requires its
                     * attention
                     */
                    std::shared_ptr<ClientBaseNotifier> baseNotifier;
                    
                };
            } // namespace External
        }     // namespace DeviceClient
    }   // namespace Iot
} // namespace Aws

#endif // DEVICE_CLIENT_EXTERNALFEATURE_H
