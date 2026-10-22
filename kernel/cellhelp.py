#!/usr/bin/env python3

ತಃ __future__ ಆಮದು annotations
ಆಮದು fileinput
ಆಮದು json
ತಃ pathlib ಆಮದು ಮಾರ್ಗ

ವರ್ಗ SimHelper:
    name: str = ""
    title: str = ""
    ports: str = ""
    ಮೂಲ: str = ""
    desc: list[str]
    ಸಂಕೇತ_2: list[str]
    ಗುಂಪು: str = ""
    ver: str = "1"
    ಟ್ಯಾಗುಗಳು: list[str]

    ವ್ಯಾಖ್ಯಾನ __init__(self) -> ಏನೂಇಲ್ಲ:
        self.desc = []
        self.ಟ್ಯಾಗುಗಳು = []

    ವ್ಯಾಖ್ಯಾನ __str__(self) -> str:
        printed_fields = [
            "name", "title", "ports", "source", "desc", "code", "group", "ver",
            "tags",
        ]
        # generate C++ struct
        val = f"cell_help[{json.dumps(self.name)}] = "
        val += "{\n"
        ಗಾಗಿ ಕ್ಷೇತ್ರ ಒಳಗೆ printed_fields:
            ಕ್ಷೇತ್ರ_val = getattr(self, ಕ್ಷೇತ್ರ)
            ಆದರೆ isinstance(ಕ್ಷೇತ್ರ_val, list):
                ಕ್ಷೇತ್ರ_val = "\n".join(ಕ್ಷೇತ್ರ_val)
            ಕ್ಷೇತ್ರ_val = ಕ್ಷೇತ್ರ_val.strip()
            val += f'  {json.dumps(field_val)},\n'
        val += "};\n"
        ಹಿಂತಿರುಗಿ val

ವ್ಯಾಖ್ಯಾನ simcells_reparse(cell: SimHelper):
    # cut manual signature
    cell.desc = cell.desc[3:]

    # code-block truth table
    new_desc = []
    indent = ""
    ಗಾಗಿ ಸಾಲು ಒಳಗೆ cell.desc:
        ಆದರೆ ಸಾಲು.startswith("Truth table:"):
            indent = "   "
            new_desc.ತೆಗೆ_2()
            new_desc.extend(["::", ""])
        new_desc.append(indent + ಸಾಲು)
    cell.desc = new_desc

    # set version
    cell.ver = "2a"

simHelper = SimHelper()

ಗಾಗಿ ಸಾಲು ಒಳಗೆ fileinput.ಒಳಹರಿವು():
    ಸಾಲು = ಸಾಲು.rstrip()
    # special comments
    ಆದರೆ ಸಾಲು.startswith("//-"):
        simHelper.desc.append(ಸಾಲು[4:] ಆದರೆ ಉದ್ದ(ಸಾಲು) > 4 ಇಲ್ಲದಿದ್ದರೆ "")
    ಅಥವಾಆದರೆ ಸಾಲು.startswith("//* "):
        _, ಕೀಲಿ, val = ಸಾಲು.ವಿಭಜಿಸು(maxsplit=2)
        setattr(simHelper, ಕೀಲಿ, val)

    # code parsing
    ಆದರೆ ಸಾಲು.startswith("module "):
        clean_ಸಾಲು = ಸಾಲು[7:].replace("\\", "").replace(";", "")
        simHelper.name, simHelper.ports = clean_ಸಾಲು.ವಿಭಜಿಸು(maxsplit=1)
        simHelper.ಸಂಕೇತ_2 = []
        short_filename = ಮಾರ್ಗ(fileinput.filename()).name
        simHelper.ಮೂಲ = f'{short_filename}:{fileinput.filelineno()}'
    ಅಥವಾಆದರೆ ಅಲ್ಲ ಸಾಲು.startswith("endmodule"):
        ಸಾಲು = "    " + ಸಾಲು
    ಪ್ರಯತ್ನಿಸು:
        simHelper.ಸಂಕೇತ_2.append(ಸಾಲು.replace("\t", "    "))
    ಅಪವಾದ AttributeError:
        # no module definition, ignore line
        ದಾಟು
    ಆದರೆ ಸಾಲು.startswith("endmodule"):
        short_filename = ಮಾರ್ಗ(fileinput.filename()).name
        ಆದರೆ simHelper.ver == "1" ಮತ್ತು short_filename == "simcells.v":
            # default simcells parsing
            simcells_reparse(simHelper)

        # check help
        ಆದರೆ simHelper.desc ಮತ್ತು simHelper.ver == "1" ಮತ್ತು short_filename == "simlib.v" ಮತ್ತು simHelper.desc[1].startswith('    '):
            simHelper.desc.ತೆಗೆ_2(1)

        # check group
        ಪ್ರತಿಜ್ಞಾ simHelper.ಗುಂಪು, f"techlibs/common/{simHelper.source}: {simHelper.name} cell missing group"

        # dump
        ಮುದ್ರಿಸು(simHelper)
        # new
        simHelper = SimHelper()
