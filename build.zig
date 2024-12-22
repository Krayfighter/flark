
const std = @import("std");


pub fn build(builder: *std.Build) void {
	// const windows = builder.option(
	// 	bool, "window_cross", "Cross Compile to windows (NOT for native windows users)"
	// ) orelse false;

	const target = builder.standardTargetOptions(.{});
	const optimize = builder.standardOptimizeOption(.{});

	// const raylib_dep = builder.lazyDependency("raylib", .{
	// 	.target = target, .optimize = optimize
	// }) orelse return;
	// const raylib = raylib_dep.artifact("raylib");
	// // builder.installArtifact(raylib);

	const flark_exe = builder.addExecutable(.{
		.name = "flark",
		// .target = builder.resolveTargetQuery(.{ .os_tag = if (windows) .windows else null }),
		.target = target,
		.optimize = optimize,
	});

	// flark_exe.dependOn(raylib);
	const sources = .{ "src/main.c", "src/level_loader.c", "src/player.c" };
	flark_exe.addCSourceFiles(.{ .files = &sources });
	// flark_exe.addObjectFile("build/lib/libraylib.a");
	flark_exe.addLibraryPath(.{ .cwd_relative = "build/lib/" });
	// flark_exe.linkLibrary("build/lib/libraylib.a");
	flark_exe.linkSystemLibrary("raylib");
	flark_exe.addIncludePath(.{ .cwd_relative = "build/include/" });
	flark_exe.addIncludePath(.{ .cwd_relative = "raylib/src/" });
	flark_exe.addIncludePath(.{ .cwd_relative = "plustypes/src" });

	if (target.result.os.tag == .windows) {
		flark_exe.linkSystemLibrary("winmm");
		flark_exe.linkSystemLibrary("gdi32");
		flark_exe.linkSystemLibrary("opengl32");
	}

	flark_exe.linkLibC();

	builder.installArtifact(flark_exe);
}


