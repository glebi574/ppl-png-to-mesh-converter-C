engines = {'rgwaa'}

pixel_size = 3
max_r_offset = 1
max_g_offset = 1
max_b_offset = 1
max_a_offset = 1
max_v_amount = 4000 -- works faster if files are smaller lol ! causes error if lower than image heigth !

function make_color(r, g, b, a)
  return ((r * 256 + g) * 256 + b) * 256 + a
end

function table.copy(t1, t2)
	for i, v in pairs(t1) do
		t2[i] = v
	end
end

function update_loading_bar(p, text)
	local c = math.floor(p * 10)
	io.write'['
	for i = 1, c do
		io.write'='
	end
	for i = 1, 10 - c do
		io.write'.'
	end
	io.write('] ' .. math.floor(p * 100) .. '% ')
	if text then
		io.write(text)
	end
	io.write("\r")
  io.flush()
end

function concatf(t, format, d)
	local str = ''
	for i = 1, #t - 1 do
		str = str .. string.format(format, t[i]) .. d
	end
	return str .. string.format(format, t[#t])
end

--[[
	
	engines - list of files, that should be used to transform created bmp array into mesh
	each one generates it's own output and depends on same bmp array
	it's the only variable required for correct work of program
	
	this file will be executed in same environment, so you can execute there anything you need for all engines ig
	
	visible vertex limit should be between 10920 and 21000, depends on engine(amount of segments)
	mesh table can store up to 28000 vertexes(also depends on engine), but only vertexes limited by previous number will be shown in single mesh
	those values are correct if segments amount is equal to or lower than vertexes amount*
	
]]--