function Resolve-Mslib64 {
	$mscl2017 = Resolve-Path "${env:ProgramFiles(x86)}\Microsoft Visual Studio\*\BuildTools\VC\Tools\MSVC\*\bin\Hostx64\x64\lib.exe" -ErrorAction SilentlyContinue
	if($mscl2017) {
		Write-Host "Found CL 2017."
		Write-Host $mscl2017
		return $mscl2017
	}
    throw 'Could not find CL 2017.'
	return $mscl2017
}
function Resolve-Mslib86 {
	$mscl2017 = Resolve-Path "${env:ProgramFiles(x86)}\Microsoft Visual Studio\*\BuildTools\VC\Tools\MSVC\*\bin\Hostx64\x86\lib.exe" -ErrorAction SilentlyContinue
	if($mscl2017) {
		Write-Host "Found CL 2017."
		Write-Host $mscl2017
		return $mscl2017
	}
    throw 'Could not find CL 2017.'
	return $mscl2017
}
$msLIB64 = Resolve-Mslib64
$msLIB86 = Resolve-Mslib64

function Resolve-VarsCL64 {
	$mscl2017 = Resolve-Path "${env:ProgramFiles(x86)}\Microsoft Visual Studio\*\Community\VC\Auxiliary\Build\vcvars64.bat" -ErrorAction SilentlyContinue
	if($mscl2017) {
		Write-Host "Found VARs 2017."
		Write-Host $mscl2017
		return $mscl2017
	}
    throw 'Could not find VARs 2017.'
	return $mscl2017
}
function Resolve-VarsCL86 {
	$mscl2017 = Resolve-Path "${env:ProgramFiles(x86)}\Microsoft Visual Studio\*\Community\VC\Auxiliary\Build\vcvars32.bat" -ErrorAction SilentlyContinue
	if($mscl2017) {
		Write-Host "Found VARs 2017."
		Write-Host $mscl2017
		return $mscl2017
	}
    throw 'Could not find VARs 2017.'
	return $mscl2017
}
$vars64 = Resolve-VarsCL64
$vars86 = Resolve-VarsCL86

mkdir tmp  -ErrorAction SilentlyContinue
mkdir tmp/include/  -ErrorAction SilentlyContinue
mkdir ../dips/lib  -ErrorAction SilentlyContinue
mkdir ../dips/bin  -ErrorAction SilentlyContinue
mkdir ../dips/lib/x64  -ErrorAction SilentlyContinue
mkdir ../dips/bin/x64  -ErrorAction SilentlyContinue
mkdir ../dips/lib/win32  -ErrorAction SilentlyContinue
mkdir ../dips/bin/win32  -ErrorAction SilentlyContinue

wget http://www.sqlite.org/2018/sqlite-amalgamation-3220000.zip -OutFile tmp/sqlite_amalgamation.zip
wget http://www.sqlite.org/2018/sqlite-dll-win64-x64-3220000.zip -OutFile tmp/sqlite-dll-win64-x64.zip
wget http://www.sqlite.org/2018/sqlite-dll-win32-x86-3220000.zip -OutFile tmp/sqlite-dll-win32-x86.zip

Expand-Archive tmp/sqlite_amalgamation.zip tmp/
Move-Item tmp/sqlite-amalgamation-3220000/*.h tmp/include/

Expand-Archive tmp/sqlite-dll-win64-x64.zip tmp/x64/bin
Expand-Archive tmp/sqlite-dll-win32-x86.zip tmp/win32/bin

& $vars64
& $msLIB64 /def:tmp/x64/bin/sqlite3.def /OUT:tmp/x64/bin/sqlite3.lib  /MACHINE:x64
& $vars86
& $msLIB86 /def:tmp/win32/bin/sqlite3.def /OUT:tmp/win32/bin/sqlite3.lib  /MACHINE:x86

rm *.zip
#copy inclue file
Copy-Item ./tmp/include/*.h ./../dips/include
#copy libs
Copy-Item ./tmp/x64/bin/*.lib ./../dips/lib/x64/
Copy-Item ./tmp/win32/bin/*.lib ./../dips/lib/win32/
#copy dll
Copy-Item ./tmp/x64/bin/*.dll ../dips/bin/x64/
Copy-Item ./tmp/win32/bin/*.dll ../dips/bin/win32/
#remove all
rm tmp/** -Force -Recurse
rm tmp