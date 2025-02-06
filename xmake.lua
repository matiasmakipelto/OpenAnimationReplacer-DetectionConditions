-- set minimum xmake version
set_xmakever("2.8.2")

-- includes
includes ("lib/commonlibsse-ng")

-- set project
set_project("OpenAnimationReplacer-DetectionPlugin")
set_version("2.1.2")
set_license("GPL-3.0")

-- set defaults
set_languages("c++23")
set_warnings("allextra")

-- set policies
set_policy("package.requires_lock", true)

-- add rules
add_rules("mode.debug", "mode.releasedbg")
add_rules("plugin.vsxmake.autoupdate")

-- targets
target("OpenAnimationReplacer-DetectionPlugin")
    -- add dependencies to target
    add_deps("commonlibsse-ng")

    -- add commonlibsse-ng plugin
    add_rules("commonlibsse-ng.plugin", {
        name = "OpenAnimationReplacer-DetectionPlugin",
        author = "Nonameron",
        description = "SKSE64 plugin adding detection conditions from papyrus extender to OpenAnimationReplacer's API"
    })

    -- add src files
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    add_includedirs("src")
    set_pcxxheader("src/pch.h")

    -- copy build files to MODS or SKYRIM paths
    after_build(function(target)
        local copy = function(env, ext)
            for _, env in pairs(env:split(";")) do
                if os.exists(env) then
                    local plugins = path.join(env, ext, "SKSE/Plugins")
                    os.mkdir(plugins)
                    os.trycp(target:targetfile(), plugins)
                    os.trycp(target:symbolfile(), plugins)
                end
            end
        end
        copy("D:/Downloads/Skyrim/Own mods/", "OAR DetectionPlugin")
        copy("D:/Downloads/Skyrim/Mod Organizer 2/mods/", "OpenAnimationReplacer - Detection plugin")
    end)

