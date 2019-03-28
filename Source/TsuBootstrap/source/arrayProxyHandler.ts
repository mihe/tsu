/// <reference types="tsu-internals" />

type Element = unknown;
type Target = Array<Element>;

class ArrayProxyHandler implements ProxyHandler<Target> {
	parentObject: object;
	parentKey: object;

	constructor(parentObject: object, parentKey: object) {
		this.parentObject = parentObject;
		this.parentKey = parentKey;
	}

	get actualArray() {
		return __getProperty(this.parentObject, this.parentKey);
	}

	_getLength() {
		return __getArrayLength(this.parentObject, this.parentKey);
	}

	_setLength(value: unknown) {
		return __setArrayLength(this.parentObject, this.parentKey, value);
	}

	_getElement(key: PropertyKey) {
		return __getArrayElement(this.parentObject, this.parentKey, key);
	}

	_setElement(key: PropertyKey, value: Element) {
		return __setArrayElement(this.parentObject, this.parentKey, key, value);
	}

	_hasIndex(index: number) {
		return index >= 0 && index < this._getLength();
	}

	getPrototypeOf(_target: Target) {
		return Array.prototype;
	}

	setPrototypeOf(_target: Target, _value: unknown) {
		return false;
	}

	isExtensible(target: Target) {
		// Will throw an error if it doesn't return the extensibility of target
		return Object.isExtensible(target);
	}

	preventExtensions(target: Target) {
		// Will throw an error if it doesn't return the inverse extensibility of target
		return !Object.isExtensible(target);
	}

	getOwnPropertyDescriptor(_target: Target, key: PropertyKey) {
		if (key === 'length') {
			return {
				value: this._getLength(),
				writable: true,
				enumerable: false,
				configurable: false
			};
		} else {
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

	defineProperty(
		_target: Target,
		_key: PropertyKey,
		_attributes: PropertyDescriptor
	) {
		return false;
	}

	has(_target: Target, key: PropertyKey) {
		return (
			(key === 'length') ||
			(key in Array.prototype) ||
			(typeof key === 'number' && this._hasIndex(key))
		);
	}

	get(_target: Target, key: PropertyKey) {
		if (key === 'length') {
			return this._getLength();
		} else if (key in Array.prototype) {
			return Array.prototype[key];
		} else {
			return this._getElement(key);
		}
	}

	set(
		_target: Target,
		key: PropertyKey,
		value: Element,
		_receiver: unknown
	) {
		if (key === 'length') {
			this._setLength(value);
			return true;
		} else {
			return !!this._setElement(key, value);
		}
	}

	deleteProperty(_target: Target, _key: PropertyKey) {
		// #hack(#mihe): There's no good way to represent delete in Blueprint.
		// Ideally this would `return false`, but things like `Array.splice`
		// makes use of this, so it would throw a `TypeError`.

		return true;
	}

	ownKeys(_target: Target) {
		const length = this._getLength();
		const indices = new Array(length);
		for (let i = 0; i < length; ++i) {
			indices[i] = String(i);
		}
		return ['length', ...indices];
	}
}

export default ArrayProxyHandler;
