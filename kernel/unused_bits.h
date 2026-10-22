#నిర్వచితం_కాకపోతే వాడని_బిట్లు_H
#నిర్వచించు వాడని_బిట్లు_H

#అన్తర్భావ "kernel/yosys.h"
#అన్తర్భావ "kernel/sigtools.h"
#అన్తర్భావ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

నిర్మాణం వాడని_బిట్లు
{
	SigMap sigmap;
	pool<RTLIL::Sig_బిట్టు> వాడిన;

	వాడని_బిట్లు(RTLIL::Module *module) : sigmap(module)
	{
		NewCellTypes ct(module->design);
		collect_వాడిన(module, ct);
	}

	వాడని_బిట్లు(RTLIL::Module *module, స్థిరరాశి NewCellTypes &ct) : sigmap(module)
	{
		collect_వాడిన(module, ct);
	}

	శూన్య collect_వాడిన(RTLIL::Module *module, స్థిరరాశి NewCellTypes &ct)
	{
		కొరకు (స్వయంచాలక cell : module->cells())
			కొరకు (స్వయంచాలక &conn : cell->connections())
				ఒకవేళ (!ct.cell_output(cell->type, conn.మొదటి))
					కూడు_వాడిన(conn.రెండవ);

		కొరకు (స్వయంచాలక wire : module->wires())
			ఒకవేళ (wire->పోర్టు_output)
				కూడు_వాడిన(wire);
	}

	శూన్య కూడు_వాడిన(స్థిరరాశి RTLIL::SigSpec &sig)
	{
		కొరకు (స్వయంచాలక బిట్టు : sigmap(sig))
			ఒకవేళ (బిట్టు.wire != nullptr)
				వాడిన.చొప్పించు(బిట్టు);
	}

	bool check(RTLIL::Sig_బిట్టు బిట్టు) స్థిరరాశి
	{
		RTLIL::Sig_బిట్టు mapped = sigmap(బిట్టు);
		తిరిగి mapped.wire != nullptr && !వాడిన.లెక్క(mapped);
	}
};

YOSYS_NAMESPACE_చివర

#అయితే_ముగింపు
