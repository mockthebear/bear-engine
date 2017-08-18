--function Timer()
--	return {SetDuration=function() end,Enable=function() end}
--end

wavemanager = {
	current = 0,
	timers = {},
	delayers = {},
	counter = {},
	waveCounter = 1,

	timeBetweenWaves = 200.0,

	pauseCounter=false,

	tcounter= 50.0
}


wavemanager.waves = {
	--[[
		Props:
			lane			<wich nale>
			amount			<amount on this current lane>
			spacing			<delay between each monster summoned on current lane>
			strenght		<strenght multiplier>
			delay			<delay to this wave start its first step>
			health			<Defines an extra health to the monster. (baseHealth + health * strenght)>
			speed			<Defines an extra speed to the monster. (baseSpeed + speed) * strenght>
			affectSpeed		<Say true or false to strenght affect the speed of the monster>
	]]
	--Wave 1
	{
		{lane=1,amount = 3,spacing = 40.0, strenght = 1},
	},

	--wave 2
	--[[{
		{lane=1,amount = 2,spacing = 40.0, strenght = 1},
		{lane=2,amount = 2,spacing = 40.0, strenght = 1},
	},
	--wave 3
	{
		{lane=1,amount = 4,spacing = 34.0, strenght = 1.1},
		{lane=2,amount = 4,spacing = 34.0, strenght = 1.1, delay = 30.0},
	},


	{
		{lane=1,amount = 1,spacing = 20.0, strenght = 2.1},
		{lane=2,amount = 1,spacing = 20.0, strenght = 2.1, delay = 30.0},
	},]]

}

function wavemanager.Summon(wData)
	g_td.MakeEnemy((wData.lane or math.random(1,5)) -1,(2 + (wData.health or 0)) * (wData.strenght or 1),(3.0 + (wData.speed or 0)) * (wData.affectSpeed and (wData.strenght) or 1),math.random(1,5))
	print("Spawn an creature with ",math.floor((2 + (wData.health or 0)) * (wData.strenght or 1)),wData.strenght)
end


function wavemanager.ProcedureNextWave()
	if not wavemanager.waves[wavemanager.waveCounter] then
		local counter = wavemanager.waveCounter
		print("Generated wave: "..counter)
		wave = {}
		for i=1, (counter > 10 and 4 or (counter > 4 and 3 or 2)) do
			wave[#wave+1] = {lane = i, spacing = 20.0,strenght = 1.0 + (counter/2.8),delay = math.random(0,i*15.0), amount = 2 + math.floor((counter/4.0)),health = counter/1.3}
		end
		wavemanager.waves[wavemanager.waveCounter] = wave
	end
end
--[[
for i=1,20 do
	wavemanager.waveCounter = i
	wavemanager.ProcedureNextWave()
	wavemanager.Summon(wavemanager.waves[i][1])
end]]

function wavemanager.Start(w)
	wavemanager.current = w

	for i=1,#wavemanager.waves[wavemanager.current] do
		local current = wavemanager.waves[wavemanager.current][i]
		wavemanager.timers[i] = Timer()
		wavemanager.delayers[i] = Timer()
		wavemanager.counter[i] = current.amount or 1

		wavemanager.timers[i]:SetDuration(current.spacing or 1.0)
		wavemanager.delayers[i]:SetDuration(current.delay or 0.0)
		wavemanager.delayers[i]:Start()
		wavemanager.timers[i]:Start()
	end
end

--wavemanager.Start(3)

function wavemanager.Clean()
	wavemanager.counter = {}
	wavemanager.delayers = {}
	wavemanager.timers = {}
end


function wavemanager.IsDone()
	if wavemanager.current == 0 then
		return true
	end
	for i=1,#wavemanager.timers do
		if (wavemanager.counter[i] > 0) then
			return false
		end
	end
	return true
end




function wavemanager.Update(dt)
	if wavemanager.pauseCounter then
		return
	end
	if wavemanager.current > 0 then
		for i=1,#wavemanager.timers do
			if (wavemanager.counter[i] >= 1) then
				wavemanager.delayers[i]:Update(dt)
				if (wavemanager.delayers[i]:HasFinished()) then
					wavemanager.timers[i]:Update(dt)
					if (wavemanager.timers[i]:HasFinished()) then
						wavemanager.timers[i]:Restart()

						local wData = wavemanager.waves[wavemanager.current][i]
						print(wavemanager.current)
						wavemanager.Summon(wData)
						wavemanager.counter[i] = wavemanager.counter[i]-1
					end
				end
			end
		end
		if wavemanager.IsDone() then
			wavemanager.current = 0
		end
	else
		if wavemanager.tcounter <= 0 then
			wavemanager.tcounter = wavemanager.timeBetweenWaves
			print("Start wave"..wavemanager.waveCounter)
			wavemanager.Start(wavemanager.waveCounter)
			wavemanager.waveCounter = wavemanager.waveCounter +1
			wavemanager.ProcedureNextWave()
		else
			wavemanager.tcounter = wavemanager.tcounter - dt
		end
	end
end


