#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಬಳಸದ_ಬಿಟ್ಟುಗಳು_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಬಳಸದ_ಬಿಟ್ಟುಗಳು_H

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"
#ಅನ್ತರ್ಭಾವ "kernel/sigtools.h"
#ಅನ್ತರ್ಭಾವ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

ರಚನೆ ಬಳಸದ_ಬಿಟ್ಟುಗಳು
{
	SigMap sigmap;
	pool<RTLIL::Sig_ಬಿಟ್> ಬಳಸಿದ;

	ಬಳಸದ_ಬಿಟ್ಟುಗಳು(RTLIL::Module *module) : sigmap(module)
	{
		NewCellTypes ct(module->design);
		collect_ಬಳಸಿದ(module, ct);
	}

	ಬಳಸದ_ಬಿಟ್ಟುಗಳು(RTLIL::Module *module, ಸ್ಥಿರಾಂಕ NewCellTypes &ct) : sigmap(module)
	{
		collect_ಬಳಸಿದ(module, ct);
	}

	ಶೂನ್ಯ collect_ಬಳಸಿದ(RTLIL::Module *module, ಸ್ಥಿರಾಂಕ NewCellTypes &ct)
	{
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ cell : module->cells())
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &conn : cell->connections())
				ಆದರೆ (!ct.cell_output(cell->type, conn.ಮೊದಲ))
					ಕೂಡಿಸು_ಬಳಸಿದ(conn.ಎರಡನೇ);

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ wire : module->wires())
			ಆದರೆ (wire->ಪೋರ್ಟ್_output)
				ಕೂಡಿಸು_ಬಳಸಿದ(wire);
	}

	ಶೂನ್ಯ ಕೂಡಿಸು_ಬಳಸಿದ(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig)
	{
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಬಿಟ್ : sigmap(sig))
			ಆದರೆ (ಬಿಟ್.wire != nullptr)
				ಬಳಸಿದ.ಸೇರಿಸು(ಬಿಟ್);
	}

	bool check(RTLIL::Sig_ಬಿಟ್ ಬಿಟ್) ಸ್ಥಿರಾಂಕ
	{
		RTLIL::Sig_ಬಿಟ್ mapped = sigmap(ಬಿಟ್);
		ಹಿಂತಿರುಗಿ mapped.wire != nullptr && !ಬಳಸಿದ.ಎಣಿಕೆ_2(mapped);
	}
};

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
