import __resolve from 'resolve/lib/sync';

function trimRoot(str) {
	return str.startsWith('/') ? str.slice(1) : str;
}

function resolve(id) {
	return trimRoot(
		__resolve(id, {
			basedir: __dirname,
			readFileSync: path => __file.read(trimRoot(path)),
			isFile: path => __file.exists(trimRoot(path))
		})
	);
}

module.exports = function (id) {
	if (id.startsWith('UE/')) {
		const name = id.slice(3);
		return { [name]: __import(name) }
	}

	return __require(resolve(id));
}
