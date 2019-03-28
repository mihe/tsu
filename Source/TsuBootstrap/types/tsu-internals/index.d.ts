declare function __getProperty(
	parentObject: object,
	parentKey: object
): unknown;

declare function __setProperty(
	parentObject: object,
	parentKey: object,
	value: unknown
): true | undefined;

declare function __getProperty(
	parentObject: object,
	parentKey: object
): void;

declare function __getArrayElement(
	parentObject: object,
	parentKey: object,
	key: PropertyKey
): Element;

declare function __setArrayElement(
	parentObject: object,
	parentKey: object,
	key: PropertyKey,
	value: Element
): true | undefined;

declare function __getArrayLength(
	parentObject: object,
	parentKey: object
): number;

declare function __setArrayLength(
	parentObject: object,
	parentKey: object,
	value: unknown
): void;

declare function __import(id: string): unknown;

declare function __require(id: string): unknown;

declare const __file: {
	read(path: string): string;
	exists(path: string): boolean;
};

declare const __path: {
	join(...parts: string[]): string;
	resolve(...parts: string[]): string;
	dirname(path: string): string;
};
