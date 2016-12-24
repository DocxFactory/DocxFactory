
var fileName	= WScript.Arguments(0),
	tempFile	= '.\\temp.cxx',

	fso			= new ActiveXObject('Scripting.FileSystemObject'),
	stIn,
	stOut,

	prefix		= "";

stOut = fso.OpenTextFile(fileName, 8);
stOut.WriteLine('');
stOut.Close();

stIn	= fso.OpenTextFile(fileName, 1);
stOut	= fso.CreateTextFile(tempFile, true);

while (true)
{
    var line = stIn.ReadLine();
	if (stIn.AtEndOfStream)
		break;

	stOut.WriteLine(line);

	if (line.indexOf('#include "dfwc.h"') !== -1)
		prefix = 'dfwc';

	else
	if (line.indexOf('#include "dfw.h"') !== -1)
		prefix = 'dfw';

	else
	if (beginsWith(line, 'SWIGEXPORT'))
	{
		var lines = [], linesRelease = [], linesJResult = [];
		while (true)
		{
			line = stIn.ReadLine();
			if (stIn.AtEndOfStream || beginsWith(line, '  return jresult;') || beginsWith(line, '}'))
				break;

			if (line.indexOf('ReleaseStringUTFChars') !== -1)
				linesRelease.push(line);

			else
			if (line.indexOf('jresult =') !== -1 && line.indexOf('NewStringUTF') !== -1)
				linesJResult.push(line);

			else
				lines.push(line);
		}
		
		for (var i = 0; i < lines.length; i++)
			stOut.WriteLine(lines[i]);
		
		for (var i = 0; i < linesRelease.length; i++)
			stOut.WriteLine(linesRelease[i]);
		
		stOut.WriteLine('  if (' + prefix + '_getErrorFlag())');
		stOut.WriteLine('    SWIG_JavaThrowException(jenv, SWIG_JavaRuntimeException, ' + prefix + '_getErrorWhat());');

		for (var i = 0; i < linesJResult.length; i++)
			stOut.WriteLine(linesJResult[i]);

		stOut.WriteLine(line);
	}
}

stIn.Close();
stOut.Close();

fso.CopyFile(tempFile, fileName, true);
fso.DeleteFile(tempFile, true);



function beginsWith(str, prefix)
{
	if (str.length < prefix.length)
		return false;

	if (str.substr(0, prefix.length) === prefix)
		return true;

	return false;
} // beginsWith
