'use strict';

class ArrayProxyHandler {
    constructor(parentObject, parentKey) {
        this.parentObject = parentObject;
        this.parentKey = parentKey;
    }
    get actualArray() {
        return __getProperty(this.parentObject, this.parentKey);
    }
    _getLength() {
        return __getArrayLength(this.parentObject, this.parentKey);
    }
    _setLength(value) {
        return __setArrayLength(this.parentObject, this.parentKey, value);
    }
    _getElement(key) {
        return __getArrayElement(this.parentObject, this.parentKey, key);
    }
    _setElement(key, value) {
        return __setArrayElement(this.parentObject, this.parentKey, key, value);
    }
    _hasIndex(index) {
        return index >= 0 && index < this._getLength();
    }
    getPrototypeOf(_target) {
        return Array.prototype;
    }
    setPrototypeOf(_target, _value) {
        return false;
    }
    isExtensible(target) {
        return Object.isExtensible(target);
    }
    preventExtensions(target) {
        return !Object.isExtensible(target);
    }
    getOwnPropertyDescriptor(_target, key) {
        if (key === 'length') {
            return {
                value: this._getLength(),
                writable: true,
                enumerable: false,
                configurable: false
            };
        }
        else {
            const element = this._getElement(key);
            if (typeof element === 'undefined') {
                return undefined;
            }
            return {
                value: element,
                writable: true,
                enumerable: true,
                configurable: false
            };
        }
    }
    defineProperty(_target, _key, _attributes) {
        return false;
    }
    has(_target, key) {
        return ((key === 'length') ||
            (key in Array.prototype) ||
            (typeof key === 'number' && this._hasIndex(key)));
    }
    get(_target, key) {
        if (key === 'length') {
            return this._getLength();
        }
        else if (key in Array.prototype) {
            return Array.prototype[key];
        }
        else {
            return this._getElement(key);
        }
    }
    set(_target, key, value, _receiver) {
        if (key === 'length') {
            this._setLength(value);
            return true;
        }
        else {
            return !!this._setElement(key, value);
        }
    }
    deleteProperty(_target, _key) {
        return true;
    }
    ownKeys(_target) {
        const length = this._getLength();
        const indices = new Array(length);
        for (let i = 0; i < length; ++i) {
            indices[i] = String(i);
        }
        return ['length', ...indices];
    }
}

module.exports = ArrayProxyHandler;
