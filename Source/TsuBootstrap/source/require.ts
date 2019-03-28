/// <reference types="tsu-internals" />
/// <reference types="tsu-resolve-sync" />

import __resolve from 'resolve/lib/sync';

function trimRoot(str: string) {
	return str.startsWith('/') ? str.slice(1) : str;
}

function resolve(id: string) {
	return trimRoot(
		__resolve(id, {
			basedir: __dirname,
			readFileSync: (path: string) => __file.read(trimRoot(path)),
			isFile: (path: string) => __file.exists(trimRoot(path))
		})
	);
}

function _require(id: string) {
	if (id.startsWith('UE/')) {
		const name = id.slice(3);
		return { [name]: __import(name) }
	}

	return __require(resolve(id));
}

export default _require;
