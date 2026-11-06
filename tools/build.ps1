[CmdletBinding()]
param(
    [ValidateSet('Debug', 'Release', 'RelWithDebInfo', 'MinSizeRel')]
    [string]$Configuration = 'Debug'
)

$ErrorActionPreference = 'Stop'
$sourceDir = Split-Path -Parent $PSScriptRoot
$buildDir = Join-Path $sourceDir "build/ninja/$Configuration"

foreach ($command in @('cmake', 'ctest', 'ninja', 'clang++', 'clang-scan-deps')) {
    if (-not (Get-Command $command -CommandType Application -ErrorAction SilentlyContinue)) {
        throw "Required build tool '$command' was not found on PATH."
    }
}

& cmake -S $sourceDir -B $buildDir -G Ninja `
    '-DCMAKE_CXX_COMPILER=clang++' "-DCMAKE_BUILD_TYPE=$Configuration" `
    '-DWERELIB_BUILD_TESTS=ON'
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

& cmake --build $buildDir --parallel
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

& ctest --test-dir $buildDir --output-on-failure
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
