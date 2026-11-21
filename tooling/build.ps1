#Requires -Version 7.4

[CmdletBinding()]
param(
    [ValidateSet('--debug', '--release')]
    [string]$Configuration = '--debug'
)
$ErrorActionPreference = 'Stop'
$PSNativeCommandUseErrorActionPreference = $true

$buildType = $Configuration.TrimStart('-').ToLowerInvariant()
$sourceDir = Split-Path -Parent $PSScriptRoot
$buildDir = Join-Path $sourceDir "build/ninja/$buildType"

& cmake -S $sourceDir -B $buildDir -G Ninja `
    '-DCMAKE_CXX_COMPILER=clang++' "-DCMAKE_BUILD_TYPE=$buildType"

& cmake --build $buildDir
