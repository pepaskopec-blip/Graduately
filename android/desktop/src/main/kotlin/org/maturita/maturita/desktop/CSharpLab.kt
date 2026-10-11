package org.maturita.maturita.desktop

import org.maturita.maturita.platform.CSharpRun
import java.io.ByteArrayOutputStream
import java.io.File
import java.nio.charset.StandardCharsets
import java.util.concurrent.TimeUnit

/** One cached console project. The student's program replaces Program.cs and `dotnet run` executes it. */
object CSharpLab {
    private val gate = Object()

    fun run(source: String, stdin: String): CSharpRun {
        if (source.length > 100_000) return CSharpRun("fail", "Program je příliš dlouhý.")
        val dotnet = findDotnet() ?: return CSharpRun("unavailable", "")
        synchronized(gate) {
            return try {
                execute(dotnet, source, stdin)
            } catch (e: Exception) {
                CSharpRun("fail", e.message ?: "Spuštění selhalo.")
            }
        }
    }

    private fun execute(dotnet: String, source: String, stdin: String): CSharpRun {
        val dir = File(System.getProperty("user.home"), ".graduately/csharp-lab")
        dir.mkdirs()
        val csproj = File(dir, "Lab.csproj")
        if (!csproj.isFile) {
            csproj.writeText(projectFile(targetFramework(dotnet)), StandardCharsets.UTF_8)
            val restored = command(dotnet, listOf("restore", dir.absolutePath), dir, "", 90)
            if (restored.status != "ok") return restored
        }
        File(dir, "Program.cs").writeText(source, StandardCharsets.UTF_8)
        val args = mutableListOf("run", "--project", dir.absolutePath, "--verbosity", "quiet")
        if (File(dir, "obj").isDirectory) args += "--no-restore"
        return command(dotnet, args, dir, stdin, 20)
    }

    private fun command(dotnet: String, args: List<String>, dir: File, stdin: String, seconds: Long): CSharpRun {
        val process = ProcessBuilder(listOf(dotnet) + args)
            .directory(dir)
            .redirectErrorStream(true)
            .apply {
                environment().clear()
                environment().putAll(environmentFor(dotnet))
            }
            .start()
        process.outputStream.use { it.write(stdin.toByteArray(StandardCharsets.UTF_8)) }
        val output = ByteArrayOutputStream()
        val reader = Thread {
            process.inputStream.use { it.copyTo(output) }
        }
        reader.isDaemon = true
        reader.start()
        val finished = process.waitFor(seconds, TimeUnit.SECONDS)
        if (!finished) {
            process.descendants().forEach { it.destroyForcibly() }
            process.destroyForcibly()
            process.waitFor(2, TimeUnit.SECONDS)
            reader.join(1000)
            return CSharpRun("timeout", clip(output.toString(StandardCharsets.UTF_8)))
        }
        reader.join(2000)
        val text = clip(output.toString(StandardCharsets.UTF_8)).trim()
        return if (process.exitValue() == 0) CSharpRun("ok", text) else CSharpRun("fail", text)
    }

    private fun clip(text: String): String =
        if (text.length <= 16_000) text else text.take(16_000) + "\n…"

    private fun environmentFor(dotnet: String): Map<String, String> {
        val home = System.getProperty("user.home")
        val root = File(dotnet).parent
        val path = listOf(root, "/opt/homebrew/bin", "/usr/local/bin", "/usr/bin", "/bin")
            .joinToString(File.pathSeparator)
        val env = mutableMapOf(
            "PATH" to path,
            "HOME" to home,
            "USERPROFILE" to home,
            "DOTNET_ROOT" to root,
            "DOTNET_CLI_TELEMETRY_OPTOUT" to "1",
            "DOTNET_NOLOGO" to "1",
            "DOTNET_SKIP_FIRST_TIME_EXPERIENCE" to "1",
            "TEMP" to System.getProperty("java.io.tmpdir"),
            "TMP" to System.getProperty("java.io.tmpdir"),
        )
        System.getenv("SystemRoot")?.let { env["SystemRoot"] = it }
        System.getenv("ComSpec")?.let { env["ComSpec"] = it }
        return env
    }

    private fun targetFramework(dotnet: String): String {
        val version = command(dotnet, listOf("--version"), File(System.getProperty("java.io.tmpdir")), "", 20)
        val major = version.output.trim().substringBefore('.').filter { it.isDigit() }
        return if (major.isEmpty()) "net8.0" else "net$major.0"
    }

    private fun projectFile(framework: String) = """
        <Project Sdk="Microsoft.NET.Sdk">
          <PropertyGroup>
            <OutputType>Exe</OutputType>
            <TargetFramework>$framework</TargetFramework>
            <ImplicitUsings>enable</ImplicitUsings>
            <Nullable>disable</Nullable>
            <LangVersion>latest</LangVersion>
          </PropertyGroup>
        </Project>
    """.trimIndent()

    private fun findDotnet(): String? {
        val home = System.getProperty("user.home")
        val programFiles = System.getenv("ProgramFiles") ?: "C:\\Program Files"
        val names = listOf(
            "/usr/local/share/dotnet/dotnet",
            "/usr/share/dotnet/dotnet",
            "/opt/homebrew/share/dotnet/dotnet",
            "$home/.dotnet/dotnet",
            "$programFiles\\dotnet\\dotnet.exe",
        )
        names.firstOrNull { File(it).canExecute() }?.let { return it }
        val path = System.getenv("PATH") ?: return null
        return path.split(File.pathSeparator)
            .map { File(it, if (System.getProperty("os.name").startsWith("Windows")) "dotnet.exe" else "dotnet") }
            .firstOrNull { it.canExecute() }
            ?.absolutePath
    }
}
