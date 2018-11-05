#pragma once

#include <functional>
#include <vector>
#include "Arduino.h"
#include "StreamingOperator.hpp"
#include "Homie/Datatypes/Interface.hpp"
#include "Homie/Datatypes/Callbacks.hpp"
#include "Homie/Limits.hpp"
#include "HomieRange.hpp"

class HomieNode;

namespace HomieInternals {
class HomieClass;
class Property;
class BootNormal;
class BootConfig;
class SendingPromise;

class PropertyInterface {
  friend ::HomieNode;

 public:
  PropertyInterface();

  void settable(const PropertyInputHandler& inputHandler = [](const HomieRange& range, const String& value) { return false; });

 private:
  PropertyInterface& setProperty(Property* property);

  Property* _property;
};

class Property {
  friend BootNormal;

 public:
  explicit Property(const char* id, const char* name = "", const char* datatype = "", const char* unit = "",
                    const char* format = "") {
    _id = strdup(id); _name = strdup(name); _unit = strdup(unit), _datatype = strdup(datatype),
    _format = strdup(format); _settable = false; }
  void settable(const PropertyInputHandler& inputHandler) { _settable = true;  _inputHandler = inputHandler; }

 private:
  const char* getProperty() const { return _id; }
  const char* getName() const { return _name; }
  const char* getUnit() const { return _unit; }
  const char* getDatatype() const { return _datatype; }
  const char* getFormat() const { return _format; }
  bool isSettable() const { return _settable; }
  PropertyInputHandler getInputHandler() const { return _inputHandler; }
  const char* _id;
  const char* _name;
  const char* _unit;
  const char* _datatype;
  const char* _format;
  bool _settable;
  PropertyInputHandler _inputHandler;
};
}  // namespace HomieInternals

class HomieNode {
  friend HomieInternals::HomieClass;
  friend HomieInternals::BootNormal;
  friend HomieInternals::BootConfig;

 public:
  HomieNode(const char* id, const char* name, const char* type, bool range = false, uint16_t lower = 0, uint16_t upper = 0, const HomieInternals::NodeInputHandler& nodeInputHandler = [](const HomieRange& range, const String& property, const String& value) { return false; });
  virtual ~HomieNode();

  const char* getId() const { return _id; }
  const char* getType() const { return _type; }
  const char* getName() const {return _name; }
  bool isRange() const { return _range; }
  uint16_t getLower() const { return _lower; }
  uint16_t getUpper() const { return _upper; }

  HomieInternals::PropertyInterface& advertise(const char* id);
  HomieInternals::PropertyInterface& advertise(const char* id, const char* name);
  HomieInternals::PropertyInterface& advertise(const char* id, const char* name, const char* datatype);
  HomieInternals::PropertyInterface& advertise(const char* id, const char* name, const char* datatype, const char* unit);
  HomieInternals::PropertyInterface& advertise(const char* id, const char* name, const char* datatype, const char* unit, const char* format);
  HomieInternals::SendingPromise& setProperty(const String& property) const;

 protected:
  virtual void setup() {}
  virtual void loop() {}
  virtual void onReadyToOperate() {}
  virtual bool handleInput(const HomieRange& range, const String& property, const String& value);

 private:
  const std::vector<HomieInternals::Property*>& getProperties() const;

  static HomieNode* find(const char* id) {
    for (HomieNode* iNode : HomieNode::nodes) {
      if (strcmp(id, iNode->getId()) == 0) return iNode;
    }

    return 0;
  }

  const char* _id;
  const char* _name;
  const char* _type;
  bool _range;
  uint16_t _lower;
  uint16_t _upper;
  std::vector<HomieInternals::Property*> _properties;
  HomieInternals::NodeInputHandler _inputHandler;

  HomieInternals::PropertyInterface _propertyInterface;

  static std::vector<HomieNode*> nodes;
};
