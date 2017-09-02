function add_text(){
	document.getElementById('Instruction1').textContent="lui $r2,0x00000000";
	document.getElementById('Instruction2').textContent="ori $r2,$r2,0x00000004";
	document.getElementById('Instruction3').textContent="lui $r2,0x00000000";
	document.getElementById('Instruction4').textContent="lw $r4,0x00000000($r8)";
	document.getElementById('Instruction5').textContent="ori $r8,$r1,0x00000000";
}
