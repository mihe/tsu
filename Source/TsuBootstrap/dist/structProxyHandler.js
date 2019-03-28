'use strict';

class StructProxyHandler {
    constructor(parentObject, parentKey) {
        this.parentObject = parentObject;
        this.parentKey = parentKey;
    }
    get actualObject() {
        return __getProperty(this.parentObject, this.parentKey);
    }
    set actualObject(value) {
        __setProperty(this.parentObject, this.parentKey, value);
    }
    getPrototypeOf(_target) {
        return Object.getPrototypeOf(this.actualObject);
    }
    setPrototypeOf(_target, value) {
        return Object.setPrototypeOf(this.actualObject, value);
    }
    isExtensible(_target) {
        return Object.isExtensible(this.actualObject);
    }
    preventExtensions(_target) {
        const { actualObject } = this;
        Object.preventExtensions(actualObject);
        return !this.isExtensible(actualObject);
    }
    getOwnPropertyDescriptor(_target, key) {
        return Object.getOwnPropertyDescriptor(this.actualObject, key);
    }
    defineProperty(_target, key, attributes) {
        return Object.defineProperty(this.actualObject, key, attributes);
    }
    has(_target, key) {
        return (key in this.actualObject);
    }
    get(_target, key) {
        return this.actualObject[key];
    }
    set(_target, key, value) {
        const actualObject = this.actualObject;
        actualObject[key] = value;
        this.actualObject = actualObject;
        return true;
    }
    deleteProperty(_target, key) {
        return (delete this.actualObject[key]);
    }
    ownKeys(_target) {
        return Reflect.ownKeys(this.actualObject);
    }
}

module.exports = StructProxyHandler;
