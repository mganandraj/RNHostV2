#include "precomp.h"
#include <reka/PropertyBag.h>
#include "PropertyBag_internal.h"

namespace Mso { namespace JSHost {

//=============================================================================================
// PropertyBag implementation
//=============================================================================================

LIBLET_PUBLICAPI PropertyBag::PropertyBag() noexcept
	: m_content{Mso::Make<ThreadSafePropertyBag>()}
{
}

LIBLET_PUBLICAPI PropertyBag::PropertyBag(std::nullptr_t) noexcept
{
}

LIBLET_PUBLICAPI PropertyBag::PropertyBag(Mso::TCntPtr<IPropertyBag>&& content) noexcept
	: m_content{std::move(content)}
{
}

LIBLET_PUBLICAPI PropertyBag::PropertyBag(const PropertyBag& other) noexcept
	: m_content{other.m_content}
{
}

LIBLET_PUBLICAPI PropertyBag::PropertyBag(PropertyBag&& other) noexcept
	: m_content{std::move(other.m_content)}
{
}

LIBLET_PUBLICAPI PropertyBag::~PropertyBag() noexcept
{
}

LIBLET_PUBLICAPI PropertyBag& PropertyBag::operator=(const PropertyBag& other) noexcept
{
	m_content = other.m_content;
	return *this;
}

LIBLET_PUBLICAPI PropertyBag& PropertyBag::operator=(PropertyBag&& other) noexcept
{
	m_content = std::move(other.m_content);
	return *this;
}

LIBLET_PUBLICAPI void PropertyBag::Remove(const char* propertyName) const noexcept
{
	m_content->Replace(nullptr, propertyName, nullptr);
}

LIBLET_PUBLICAPI void PropertyBag::Remove(const void* isolationKey, const char* propertyName) const noexcept
{
	m_content->Replace(isolationKey, propertyName, nullptr);
}

//=============================================================================================
// IsolatedIdComparer implementation
//=============================================================================================

bool IsolatedIdComparer::operator()(const IsolatedId& left, const IsolatedId& right) const noexcept
{
	if (left.IsolationKey < right.IsolationKey)
	{
		return true;
	}
	else if (left.IsolationKey > right.IsolationKey)
	{
		return false;
	}
	else
	{
		return left.PropertyName < right.PropertyName;
	}
}

//=============================================================================================
// ThreadSafePropertyBag implementation
//=============================================================================================

Mso::TCntPtr<IUnknown> ThreadSafePropertyBag::GetOrCreate(const void* isolationKey, const char* propertyName,
	const Mso::FunctorRef<Mso::TCntPtr<IUnknown>()>& valueMaker) noexcept
{
	IsolatedId isolatedId{reinterpret_cast<intptr_t>(isolationKey), std::string{propertyName}};

	Mso::TCntPtr<IUnknown> result = GetInternal(isolatedId);

	if (!result)
	{
		result = valueMaker();
		if (result)
		{
			std::lock_guard<std::mutex> locker(m_lock);
			// std::map operator[] creates a new entry if it is not found
			auto& entry = m_isolatedObjects[isolatedId];
			if (!entry)
			{
				entry = result;
			}
			else
			{
				// Entry is not empty - return existing value
				result = entry;
			}
		}
	}

	return result;
}

Mso::TCntPtr<IUnknown> ThreadSafePropertyBag::Get(const void* isolationKey, const char* propertyName) const noexcept
{
	return GetInternal({reinterpret_cast<intptr_t>(isolationKey), std::string{propertyName}});
}

Mso::TCntPtr<IUnknown> ThreadSafePropertyBag::GetInternal(const IsolatedId& isolatedId) const noexcept
{
	std::lock_guard<std::mutex> locker(m_lock);
	auto it = m_isolatedObjects.find(isolatedId);
	return (it != m_isolatedObjects.end()) ? it->second : nullptr;
}

Mso::TCntPtr<IUnknown> ThreadSafePropertyBag::Replace(const void* isolationKey, const char* propertyName,
	Mso::TCntPtr<IUnknown>&& value) noexcept
{
	IsolatedId isolatedId{reinterpret_cast<intptr_t>(isolationKey), std::string{propertyName}};

	std::lock_guard<std::mutex> locker(m_lock);
	// std::map operator[] creates a new entry if it is not found
	Mso::TCntPtr<IUnknown>& entry = m_isolatedObjects[isolatedId];
	Mso::TCntPtr<IUnknown> oldValue = std::move(entry);
	entry = std::move(value);
	return oldValue;
}

//=============================================================================================
// WeakPropertyBagProxy implementation
//=============================================================================================

WeakPropertyBagProxy::WeakPropertyBagProxy(const PropertyBag& propertyBag) noexcept
	: m_weakPropertyBag{GetIPropertyBag(propertyBag)}
{
}

Mso::TCntPtr<IUnknown> WeakPropertyBagProxy::GetOrCreate(const void* isolationKey, const char* propertyName,
	const Mso::FunctorRef<Mso::TCntPtr<IUnknown>()>& valueMaker) noexcept
{
	if (auto propertyBag = m_weakPropertyBag.GetStrongPtr())
	{
		return propertyBag->GetOrCreate(isolationKey, propertyName, valueMaker);
	}
	return nullptr;
}

Mso::TCntPtr<IUnknown> WeakPropertyBagProxy::Get(const void* isolationKey, const char* propertyName) const noexcept
{
	if (auto propertyBag = m_weakPropertyBag.GetStrongPtr())
	{
		return propertyBag->Get(isolationKey, propertyName);
	}
	return nullptr;
}

Mso::TCntPtr<IUnknown> WeakPropertyBagProxy::Replace(const void* isolationKey, const char* propertyName,
	Mso::TCntPtr<IUnknown>&& value) noexcept
{
	if (auto propertyBag = m_weakPropertyBag.GetStrongPtr())
	{
		return propertyBag->Replace(isolationKey, propertyName, std::move(value));
	}
	return nullptr;
}

//=============================================================================================
// PropertyBagLocalProxy implementation
//=============================================================================================

PropertyBagLocalProxy::PropertyBagLocalProxy(IPropertyBag& propertyBag) noexcept
	: m_propertyBag{propertyBag}
{
}

Mso::TCntPtr<IUnknown> PropertyBagLocalProxy::Replace(const void* isolationKey, const char* propertyName,
	Mso::TCntPtr<IUnknown>&& value) noexcept
{
	return m_propertyBag.Replace(isolationKey, propertyName, std::move(value));
}

Mso::TCntPtr<IUnknown> PropertyBagLocalProxy::Get(const void* isolationKey, const char* propertyName) const noexcept
{
	return m_propertyBag.Get(isolationKey, propertyName);
}

Mso::TCntPtr<IUnknown> PropertyBagLocalProxy::GetOrCreate(const void* isolationKey, const char* propertyName,
	const Mso::FunctorRef<Mso::TCntPtr<IUnknown>()>& valueMaker) noexcept
{
	return m_propertyBag.GetOrCreate(isolationKey, propertyName, valueMaker);
}

}} // namespace Mso::JSHost
