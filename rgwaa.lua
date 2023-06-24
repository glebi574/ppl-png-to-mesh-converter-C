
	-- Rectangle Grid With Alpha Alternation
	
local __bmp = {}
for i, v in ipairs(bmp) do
	table.insert(__bmp, {})
	table.copy(v, __bmp[i])
end
local bmp = __bmp
local time = os.clock()

for index, pixel in ipairs(bmp) do
	pixel.x = pixel.x + 1
	pixel.y = pixel.y + 1
end

local width, heigth = bmp[#bmp].x, bmp[#bmp].y
local r_amount = math.floor(heigth * max_v_amount / #bmp)
local cp = r_amount * width

local sWidth = width * pixel_size
local sHeigth = heigth * pixel_size

local x_offset = -sWidth / 2
local y_offset = -sHeigth / 2

local mesh = io.open('output/rgwaa/img/1.lua', 'w')
local r = {v = {}, s = {}, c = {}}
local mi = 1
local i_offset = 0

for i, pixel in ipairs(bmp) do
	table.insert(r.v, {pixel.x * pixel_size + x_offset, pixel.y * pixel_size + y_offset})
	if i % 2 == 0 then pixel.a = math.floor(pixel.a / 2) end
	table.insert(r.c, make_color(pixel.r, pixel.g, pixel.b, pixel.a))
	local index = i - i_offset
	if pixel.y % r_amount ~= 0 then --doesn't work sometimes... idk why...
		table.insert(r.s, {index - 1, index + width - 1})
	end
	if pixel.x ~= width then
		table.insert(r.s, {index - 1, index})
	end
	if i % cp == 0 or i == #bmp then
		update_loading_bar(i / #bmp, os.clock() - time)
		mesh:write('meshes={{vertexes={')
		local str = ''
		for _, q in ipairs(r.v) do
			str = str .. '{' .. concatf(q, '%f', ',') .. '},'
		end
		str = str .. '},segments={'
		for _, q in ipairs(r.s) do
			str = str .. '{' .. table.concat(q, ',') .. '},'
		end
		mesh:write(str .. '},colors={' .. concatf(r.c, '0x%x', ',') .. '}}}')
		mesh:close()
		r.v, r.s, r.c = {}, {}, {}
		if i == #bmp then
			break
		end
		mi = mi + 1
		i_offset = (mi - 1) * cp
		mesh = io.open('output/rgwaa/img/' .. mi .. '.lua', 'w')
	end
end
print''

local main = io.open('output/rgwaa/mesh_handler.lua', 'w')
main:write(
	'local mesh_amount = ' .. mi ..
	'\nfor i = 1, mesh_amount do' ..
	'\n	local id = pewpew.new_customizable_entity(0fx, 0fx)' ..
	'\n	pewpew.customizable_entity_set_mesh(id, \'/dynamic/img/\' .. i ..\'.lua\', 0)' ..
	'\nend'
)
main:close()