import ast
import sys

class PythonToUCPPTranslator(ast.NodeVisitor):
    def __init__(self):
        self.cpp_code = []

    def translate(self, python_file, output_file):
        with open(python_file, 'r') as file:
            python_code = file.read()

        tree = ast.parse(python_code)
        self.visit(tree)

        with open(output_file, 'w') as file:
            file.write('\n'.join(self.cpp_code))

    def visit_ClassDef(self, node):
        base_class = node.bases[0].id if node.bases else "UObject"
        class_name = node.name
        self.cpp_code.append(f"class {class_name} : public {base_class} {{")
        self.cpp_code.append("    GENERATED_BODY()")
        self.cpp_code.append("public:")
        for body_item in node.body:
            self.visit(body_item)
        self.cpp_code.append("};\n")

    def visit_FunctionDef(self, node):
        func_name = node.name
        self.cpp_code.append(f"    void {func_name}() {{}}")

if __name__ == "__main__":
    python_file = sys.argv[1]
    output_file = sys.argv[2]
    translator = PythonToUCPPTranslator()
    translator.translate(python_file, output_file)
