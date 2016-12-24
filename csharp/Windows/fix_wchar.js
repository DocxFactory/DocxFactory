
var fileName	= '.\\WordProcessingMerger.cs',
	tempFile	= '.\\temp.cxx',

	fso			= new ActiveXObject('Scripting.FileSystemObject'),
	stIn,
	stOut;

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

	if (line.indexOf('global::System.IntPtr cPtr = ') !== -1)
	{
		line = line.replace('global::System.IntPtr cPtr = ', 'string ret = ');

		stIn.ReadLine();
	}

	else
	if (line.indexOf('SWIGTYPE_p_wchar_t.getCPtr(') !== -1)
	{
		line = line.replace(/SWIGTYPE_p_wchar_t\.getCPtr\(\w*\)/g, function(str)
		{
			return str.substring(str.indexOf('(') + 1, str.lastIndexOf(')'));
		});
	}

	else
	if (line.indexOf('SWIGTYPE_p_wchar_t') !== -1)
	{
		line = line.replace(/SWIGTYPE_p_wchar_t/g, 'string');
	}

	stOut.WriteLine(line);
}

stIn.Close();
stOut.Close();

fso.CopyFile(tempFile, fileName, true);
fso.DeleteFile(tempFile, true);



fileName	= '.\\WordProcessingMergerPINVOKE.cs';

stOut = fso.OpenTextFile(fileName, 8);
stOut.WriteLine('');
stOut.Close();

stIn		= fso.OpenTextFile(fileName, 1);
stOut		= fso.CreateTextFile(tempFile, true);

lines = [];

while (true)
{
    var line = stIn.ReadLine();
	if (stIn.AtEndOfStream)
		break;

	lines.push(line);
}

for (var i = 0; i < lines.length; i++)
{
	line = lines[i];
	
	if (line.indexOf('global::System.Runtime.InteropServices.HandleRef') !== -1) {
		line = line.replace(/global::System\.Runtime\.InteropServices\.HandleRef/g, 'string');
		lines[i] = line;

		line = lines[i - 1];
		line = line.substring(0, line.lastIndexOf(')]')) + ', CharSet=CharSet.Unicode)]';
		lines[i - 1] = line;
	}
	
	else
	if (line.indexOf('global::System.IntPtr') !== -1) {
		line = line.replace(/global::System\.IntPtr/g, 'string');
		lines[i] = line;

		line = lines[i - 1];
		line = line.substring(0, line.lastIndexOf(')]')) + ', CharSet=CharSet.Unicode)]';
		lines[i - 1] = line;
	}
}

stOut.WriteLine('using System.Runtime.InteropServices;');
stOut.WriteLine('');

for (var i = 0; i < lines.length; i++)
{
	stOut.WriteLine(lines[i]);
}

stIn.Close();
stOut.Close();

fso.CopyFile(tempFile, fileName, true);
fso.DeleteFile(tempFile, true);
