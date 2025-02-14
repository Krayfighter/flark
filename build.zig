
const std = @import("std");


pub fn build(builder: *std.Build) void {

	const target = builder.standardTargetOptions(.{});
	const optimize = builder.standardOptimizeOption(.{});

	var new_malloc = builder.addSharedLibrary(.{
		.name = "new_malloc",
		.target = target,
		.optimize = optimize,
	});
	new_malloc.linkLibC();
	new_malloc.addCSourceFiles(.{ .files =  &.{ "../plustypes/new_malloc.c", "../plustypes/new_malloc_front.c" } });

	var flark_exe = builder.addExecutable(.{
		.name = "flark",
		.target = target,
		.optimize = optimize,
	});
	const sources = .{ "src/main.c", "src/geometry.c", "src/text.c" };
	flark_exe.addCSourceFiles(.{ .files = &sources });
	flark_exe.linkLibrary(new_malloc);
	flark_exe.setVerboseCC(true);
	flark_exe.setVerboseLink(true);
	flark_exe.linkLibC();
	flark_exe.linkSystemLibrary("sdl3");

	if (optimize == .Debug) {
		flark_exe.defineCMacro("DEBUG_BUILD", null);
	}

	builder.installArtifact(flark_exe);
	
}


