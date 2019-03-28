/// <reference types="tsu-internals" />

type Struct = object;
type Target = Struct;

class StructProxyHandler implements ProxyHandler<Target> {
	parentObject: object;
	parentKey: object;

	constructor(parentObject: object, parentKey: object) {
		this.parentObject = parentObject;
		this.parentKey = parentKey;
	}

	get actualObject() {
		return __getProperty(this.parentObject, this.parentKey) as object;
	}

	set actualObject(value) {
		__setProperty(this.parentObject, this.parentKey, value);
	}

	getPrototypeOf(_target: Target) {
		return Object.getPrototypeOf(this.actualObject);
	}

	setPrototypeOf(_target: Target, value: any) {
		return Object.setPrototypeOf(this.actualObject, value);
	}

	isExtensible(_target: Target) {
		return Object.isExtensible(this.actualObject);
	}

	preventExtensions(_target: Target) {
		const { actualObject } = this;
		Object.preventExtensions(actualObject);
		return !this.isExtensible(actualObject);
	}

	getOwnPropertyDescriptor(_target: Target, key: PropertyKey) {
		return Object.getOwnPropertyDescriptor(this.actualObject, key);
	}

	defineProperty(
		_target: Target,
		key: PropertyKey,
		attributes: PropertyDescriptor
	) {
		return Object.defineProperty(this.actualObject, key, attributes);
	}

	has(_target: Target, key: PropertyKey) {
		return (key in this.actualObject);
	}

	get(_target: Target, key: PropertyKey) {
		return this.actualObject[key];
	}

	set(_target: Target, key: PropertyKey, value: unknown) {
		const actualObject = this.actualObject;
		actualObject[key] = value;
		this.actualObject = actualObject;
		return true;
	}

	deleteProperty(_target: Target, key: PropertyKey) {
		return (delete this.actualObject[key]);
	}

	ownKeys(_target: Target) {
		return Reflect.ownKeys(this.actualObject);
	}
}

export default StructProxyHandler;
