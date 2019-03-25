import node from 'rollup-plugin-node-resolve';
import commonjs from 'rollup-plugin-commonjs';
import json from 'rollup-plugin-json';
import inject from 'rollup-plugin-inject';
import { builtinModules } from 'module';

const innerRequire = `function require(id) {
	switch (id) {
		case 'path': return __path;
		case 'process': return { platform: __platform };
		default: return undefined;
	}
}`

export default [{
	input: 'source/require.js',

	output: {
		file: 'output/require.js',
		format: 'cjs',
		interop: false,
		intro: innerRequire
	},

	external(id) {
		return builtinModules.includes(id);
	},

	// #hack(#mihe): We disable this so it won't cull the temporary `require`
	treeshake: false,

	plugins: [
		node(),
		commonjs(),
		json(),

		// #hack(#mihe): We force the global process variable to come from an
		// import instead, so it can be picked up by our temporary `require`
		inject({ process: 'process' })
	],

	onwarn(warning) {
		if (warning.code !== 'MISSING_NODE_BUILTINS') {
			throw new Error(warning.message);
		}
	}
}];
