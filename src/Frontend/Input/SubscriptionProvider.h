#pragma once

#include <cstdint>
#include <functional>
#include <unordered_map>

template <class T> class SubscriptionProvider {
public:
  int Subscribe(const T& callback);
  int Unsubscribe(int subscriptionId);
  bool Call(const std::function<bool(const T&)>& invokeCall);

private:
  std::unordered_map<uint16_t, T> callbacks;
  uint16_t nextCallbackId;
};

template <class T> int SubscriptionProvider<T>::Subscribe(const T& callback) {
  callbacks[nextCallbackId] = callback;
  return nextCallbackId++;
}

template <class T> int SubscriptionProvider<T>::Unsubscribe(int subscriptionId) {
  if (subscriptionId < 0 || subscriptionId >= static_cast<int>(nextCallbackId)) {
    return -1; // Invalid subscription ID
  }

  callbacks.erase(subscriptionId);
  return 0;
}

template <class T>
bool SubscriptionProvider<T>::Call(const std::function<bool(const T&)>& invokeCall) {
  for (const auto& [id, callback] : callbacks) {
    if (invokeCall(callback)) {
      return true; // Early exit if callback returns true
    }
  }
  return false;
}

