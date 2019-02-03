#include <experimental/executor>

namespace std
{
    namespace experimental::net
    {
        inline namespace v1
        {
            execution_context::execution_context() : service_registry_(make_unique<_Service_registry>(*this))
            {
            }

            execution_context::~execution_context()
            {
                shutdown();
                destroy();
            }

            void _Service_registry::shutdown_services()
            {
                unique_ptr<execution_context::service>& svc = first_service_;
                while (svc)
                {
                    svc->shutdown();
                    svc = svc->next_;
                }
            }

            void _Service_registry::destroy_services()
            {
                while (first_service_)
                {
                    first_service_ = move(first_service_->next_);
                }
            }
        } // namespace v1
    } // namespace experimental::net
} // namespace std
